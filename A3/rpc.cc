#include <string>
#include <thread>
#include <mutex>
#include <map>
#include <vector>
#include "rpc.h"
#include "binder.h"
#include "structs.h"

using namespace std;

int binderSocket = -1;
int listeningSocket = -1;
string localHostname = "";
unsigned short localPort = 0;
map<function_info, skeleton> functions;
vector<thread> runningThreads;

mutex mtx;

int rpcInit() {
	int status;
	const char *binderAddress = getenv("BINDER_ADDRESS");
    const char *binderPort = getenv("BINDER_PORT");

    if (binderAddress == NULL) {
		return NO_BINDER_ADDRESS_ENV;
	}

    if (binderPort == NULL) {
		return NO_UNSET_BINDER_PORT_ENV;
	}

    status = createSocketAndListen(localHostname, localPort, listeningSocket);
	if (statue < 0)  {
		return status;
	}

	status = createSocketAndConnect(binderAddress, binderPort, binderSocket);
	if (statue < 0)  {
		return status;
	}
	
	return 0;
}

int rpcRegister(const char *name, int *argTypes, skeleton f) {
	if (localHostname == "" || localPort == 0 || listeningSocket == -1 || binderSocket == -1) {
		return RPC_NOT_INIT;
	}
	Receiver binderReceiver(binderSocket);
	Sender binderSender(binderSocket);
	unsigned int argTypesLength = getArgTypesLength(argTypes);
	string functionName(name);
	int status = binderSender.sendRegister(localHostname, localPort, functionName, argTypes);
	if (status < 0) {
		debug_message("can't send register request");
		return SOCKET_SEND_FAILED;
	} else if (statue == 0) {
		debug_message("binder socket closed while registering");
		return SOCKET_CLOSED;
	}

	unsigned int size;
	unsigned int messageType;
	binderReceiver.receiveUnsignedInt(size);
	binderReceiver.receiveUnsignedInt(messageType);
	if (messageType == REGISTER_SUCCESS) {
		function_info functionInfo = toFunctionInfo(functionName, argTypes);
		functions[functionInfo] = f;
		return 0;
	} else if (messageType == REGISTER_FAILURE) {
		assert(size == 12);
		int reasonCode;
		binderReceiver.receiveInt(reasonCode);
		return reasonCode;
	} else {
		debug_message("unexpected message type while registering");
		return UNEXPECTED_MESSAGE;
	}
}

void handleRpcCall(int clientSocket, char *message) {
	char *bufferHead = message;
	Receiver receiver(clientSocket);
	Sender sender(clientSocket);
	char nameBuffer[FUNCTION_NAME_SIZE];
	bufferHead = extractCharArray(bufferHead, nameBuffer, FUNCTION_NAME_SIZE);
	string functionName(nameBuffer);
	unsigned int argTypesLength;
	bufferHead = extractUnsignedInt(bufferHead, argTypesLength);
	int argTypes[argTypesLength];
	void *args[argTypesLength - 1];

	skeleton f = NULL;
	function_info requested = toFunctionInfo(functionName, argTypes);
	{
		lock_guard<mutex> lock(mutex);
		for (map<function_info, skeleton>::iterator it = functions.begin(); it != functions.end(); it++) {
			if (it->first == requested) {
				f = it->second;
				break;
			}
		}
	}

	if (f == NULL) {
		sender.sendExecuteFailure(FUNCTION_NOT_FOUND);
	}
	extractArguments(bufferHead, argTypes, argTypesLength, args, true);
	int success = f(argTypes, args);
	if (success < 0) {
		sender.sendExecuteFailure(SERVER_EXECUTE_FAILED);
	} else {
		sender.sendExecuteSuccess(functionName, argTypes, args);
	}
	for (unsigned int i = 0; i < argTypesLength - 1; i++) {
		delete[] args[i];
	}
}

int processRequest(int socket bool &terminate) {
	Receiver receiver(socket);
	unsigned int size;
	receiver.receiveUnsignedInt(size);
	unsigned int messageType;
	receiver.receiveUnsignedInt(messageType);
	if (messageType == TERMINATE) {
		if (socket == binderSocket) {
			terminate = true;
			return 0;
		} else {
			return TERMINATE_NOT_FROM_BINDER;
		}
	} else {
		if (messageType != EXECUTE) {
			debug_message("unexpected message");
			return UNEXPECTED_MESSAGE;
		}
		char message = new char[size];
		receiver.receiveMessage(size, message);
		thread rpcCall(handleRpcCall, socket, message);
		runningThreads.push_back(rpcCall);
	}
}

int rpcExecute() {
	if (functions.empty()) {
		return NO_FUNCTIONS_REGISTERED;
	}
	fd_set master_fds, read_fds;
	int fdmax;
	FD_SET(listeningSocket, &master_fds);
	FD_SET(binderSocket, &master_fds);
	fdmax = listeningSocket > binderSocket ? listeningSocket : binderSocket;
	set<int> all_sockets;
	all_sockets.insert(listeningSocket);
	all_sockets.insert(binderSocket);
	boolean terminate = false;
	while (!terminate) {
		read_fds = master_fds;
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) < 0) {
			exit(1);
		}
		for (int socket : all_sockets) {
			if (FD_ISSET(socket, &read_fds)) {
				if (socket == listeningSocket) {
					int new_client_socket = accept_new_client(listeningSocket, &master_fds, all_sockets);
					if (new_client_socket > fdmax) {
						fdmax = new_client_socket;
					}
				} else {
					processClientRequest(socket, terminate);
					if (terminate) {
						break;
					}
				}
			}
		}
	}

	for (unsigned int i = 0; i < runningThreads.size(); i++) {
		thread t = runningThreads[i];
		t.join();
	}
	return 0;
}