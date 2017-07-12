#include <string>
#include <thread>
#include <mutex>
#include <map>
#include <vector>
#include <list>
#include <utility>
#include <assert.h>
#include <unistd.h>
#include <utility>
#include <iostream>
#include <bitset>
#include "helpers.h"
#include "rpc.h"
#include "structs.h"
#include "sender.h"
#include "receiver.h"
#include "buffer.h"
#include "helpers.h"

using namespace std;

int binderSocket = -1;
int listeningSocket = -1;
string localHostname = "";
unsigned short localPort = 0;
list<pair<function_info, skeleton>> functions;
vector<thread> runningThreads;

mutex mtx;

string argTypesToString(int *argTypes) {
	unsigned int argTypesLength = getArgTypesLength(argTypes);
	string str = "argTypes:";
	for (unsigned int i = 0; i < argTypesLength; i++) {
		str += " " + to_string(argTypes[i]);
	}
	return str;
}

int connectToBinder() {
	if (binderSocket != -1) {
		return 0;
	}

	const char *binderAddress = getenv("BINDER_ADDRESS");
	const char *binderPort = getenv("BINDER_PORT");

	if (binderAddress == NULL) {
		return NO_BINDER_ADDRESS_ENV;
	}

	if (binderPort == NULL) {
		return NO_BINDER_PORT_ENV;
	}

	return createSocketAndConnect(binderAddress, binderPort, binderSocket);
}

int rpcInit() {
	if (binderSocket != -1 && listeningSocket != -1) {
		return RPC_ALREADY_INIT;
	}

	int status;
	
	status = connectToBinder();
	if (status < 0) {
		return status;
	}

	status = createSocketAndListen(localHostname, localPort, listeningSocket);
	if (status < 0)  {
		return status;
	}
	debug_message("completed RPC init");
	return 0;
}

int rpcRegister(const char *name, int *argTypes, skeleton f) {
	debug_message("RPC register");
	if (localHostname == "" || localPort == 0 || listeningSocket == -1 || binderSocket == -1) {
		debug_message("RPC init not called");
		return RPC_NOT_INIT;
	}
	debug_message("binder socket is " + to_string(binderSocket));
	Receiver binderReceiver(binderSocket);
	Sender binderSender(binderSocket);
	string functionName(name);
	debug_message("sending RPC register request to binder");
	int status = binderSender.sendRegister(localHostname, localPort, functionName, argTypes);
	if (status < 0) {
		debug_message("can't send register request");
		return SOCKET_SEND_FAILED;
	} else if (status == 0) {
		debug_message("binder socket closed while registering");
		return SOCKET_CLOSED;
	}
	debug_message("send RPC register request successful");

	unsigned int size;
	unsigned int messageType;
	binderReceiver.receiveUnsignedInt(size);
	binderReceiver.receiveUnsignedInt(messageType);
	assert(size == 4);
	int reasonCode;
	binderReceiver.receiveInt(reasonCode);
	if (messageType == REGISTER_SUCCESS) {
		debug_message("got REGISTER_SUCCESS");
		function_info functionInfo = toFunctionInfo(functionName, argTypes);
		for (list<pair<function_info, skeleton>>::iterator it = functions.begin(); it != functions.end(); it++) {
			if (it->first == functionInfo) {
				functions.erase(it);
				break;
			}
		}
		pair<function_info, skeleton> newPair(functionInfo, f);
		functions.push_back(newPair);
		return reasonCode;
	} else if (messageType == REGISTER_FAILURE) {
		debug_message("got REGISTER_FAILURE");
		return reasonCode;
	} else {
		debug_message("unexpected message type while registering");
		return UNEXPECTED_MESSAGE;
	}
}

void handleRpcCall(int clientSocket, char *message) {
	debug_message("handling rpc call");
	char *bufferHead = message;
	Receiver receiver(clientSocket);
	Sender sender(clientSocket);
	char nameBuffer[FUNCTION_NAME_SIZE];
	bufferHead = extractCharArray(bufferHead, nameBuffer, FUNCTION_NAME_SIZE);
	string functionName(nameBuffer);
	unsigned int argTypesLength;
	bufferHead = extractUnsignedInt(bufferHead, argTypesLength);
	int argTypes[argTypesLength];
	bufferHead = extractIntArray(bufferHead, argTypes, argTypesLength);

	function_info requested = toFunctionInfo(functionName, argTypes);
	debug_message("got function " + to_string(requested));
	debug_message("extracting arguments");
	void *args[argTypesLength - 1];

	/*for (unsigned int i = (FUNCTION_NAME_SIZE + 4 + argTypesLength * 4); i < 12; i++) {
		if (i % 4 == 0) cout << endl;
		cout << bitset<8>(message[i]) << " ";
	}
	cout << endl;*/
	extractArguments(bufferHead, argTypes, argTypesLength, args, true);

	skeleton f = NULL;
	{
		lock_guard<mutex> lock(mutex);
		for (list<pair<function_info, skeleton>>::iterator it = functions.begin(); it != functions.end(); it++) {
			if (it->first == requested) {
				f = it->second;
				break;
			}
		}
	}

	if (f == NULL) {
		sender.sendExecuteFailure(FUNCTION_NOT_FOUND);
	}
	int success = f(argTypes, args);
	if (success < 0) {
		sender.sendExecuteFailure(SERVER_EXECUTE_FAILED);
	} else {
		sender.sendExecuteSuccess(functionName, argTypes, args);
	}
	delete[] message;
	for (unsigned int i = 0; i < argTypesLength - 1; i++) {
		delete[] (char *) args[i];
	}
}

int processRequest(int socket, bool &terminate) {
	Receiver receiver(socket);
	unsigned int size;
	int status;
	status = receiver.receiveUnsignedInt(size);
	if (status == 0) {
		return SOCKET_CLOSED;
	} else if (status < 0) {
		debug_message("receive size failed while processing request for rpc execute");
		return SOCKET_RECEIVE_FAIL;
	}
	unsigned int messageType;
	status = receiver.receiveUnsignedInt(messageType);
	if (status == 0) {
		return SOCKET_CLOSED;
	} else if (status < 0) {
		debug_message("receive type failed while processing request for rpc execute");
		return SOCKET_RECEIVE_FAIL;
	}
	if (messageType == TERMINATE) {
		if (socket == binderSocket) {
			terminate = true;
			return 0;
		} else {
			return TERMINATE_NOT_FROM_BINDER;
		}
	} else {
		if (messageType != EXECUTE) {
			debug_message("unexpected message " + to_string(messageType));
			return UNEXPECTED_MESSAGE;
		}
		char *message = new char[size];
		receiver.receiveMessage(size, message);
		thread rpcCall(handleRpcCall, socket, message);
		runningThreads.push_back(move(rpcCall));
		return 0;
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
	bool terminate = false;
	int status = 0;
	while (!terminate) {
		read_fds = master_fds;
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) < 0) {
			status = SELECT_FAIL;
			break;
		}
		for (int socket : all_sockets) {
			if (FD_ISSET(socket, &read_fds)) {
				if (socket == listeningSocket) {
					int new_client_socket = accept_new_client(listeningSocket, &master_fds, all_sockets);
					if (new_client_socket > fdmax) {
						fdmax = new_client_socket;
					}
				} else {
					status = processRequest(socket, terminate);
					if (status == SOCKET_CLOSED) {
						debug_message("closing connection to client");
						FD_CLR(socket, &master_fds);
						all_sockets.erase(socket);
						fdmax = *all_sockets.rbegin();
						close(socket);
					} else if (status < 0) {
						debug_message("rpc execute failed with status " + to_string(status));
						break;
					}
					if (terminate) {
						break;
					}
				}
			}
		}
	}

	for (unsigned int i = 0; i < runningThreads.size(); i++) {
		runningThreads.at(i).join();
	}
	close(binderSocket);
	close(listeningSocket);
	binderSocket = -1;
	listeningSocket = -1;
	localHostname = "";
	localPort = 0;
	return status;
}

int clientExecute(int socket, string name, int *argTypes, void **args) {

	debug_message("client execute for function " + name);
	Sender sender(socket);
	Receiver receiver(socket);

	int status;

	unsigned int argTypesLength = getArgTypesLength(argTypes);

	debug_message("sending execute");

	status = sender.sendExecute(name, argTypes, args);

	if (status < 0) {
		return SOCKET_SEND_FAILED;
	}
	if (status == 0) {
		return SOCKET_CLOSED;
	}

	unsigned int size;
	unsigned int type;
	status = receiver.receiveUnsignedInt(size);
	if (status < 0) {
		return SOCKET_RECEIVE_FAIL;
	}
	status = receiver.receiveUnsignedInt(type);
	if (status < 0) {
		return SOCKET_RECEIVE_FAIL;
	}

	char returnedMessage[size];
	receiver.receiveMessage(size, returnedMessage);
	close(socket);
	switch(type) {
		case EXECUTE_SUCCESS: {
			debug_message("execute success");
			char *bufferHead = returnedMessage;
			unsigned int returnedArgTypesLength;
			char nameBuffer[FUNCTION_NAME_SIZE];
			bufferHead = extractCharArray(bufferHead, nameBuffer, FUNCTION_NAME_SIZE);
			string returnName(nameBuffer);
			assert(returnName.compare(name) == 0);
			bufferHead = extractUnsignedInt(bufferHead, returnedArgTypesLength);
			assert(argTypesLength == returnedArgTypesLength);

			int returnedArgTypes[argTypesLength];
			bufferHead = extractIntArray(bufferHead, returnedArgTypes, argTypesLength);
			extractArguments(bufferHead, returnedArgTypes, argTypesLength, args, false);
			return 0;
		}

		case EXECUTE_FAILURE: {
			int returnCode;
			assert(size == 4);
			extractInt(returnedMessage, returnCode);
			return returnCode;
		}

		default:
			debug_message("received known message type from server while request execute from client: " + to_string(type));
			return UNEXPECTED_MESSAGE;
	}
}

int rpcCall(const char *name, int *argTypes, void **args) {
	debug_message("rpc call");
	string functionName(name);
	debug_message("name: " + functionName + " " + argTypesToString(argTypes));
	int status;
	connectToBinder();
	Sender binderSender(binderSocket);
	Receiver binderReceiver(binderSocket);
	status = binderSender.sendLoc(functionName, argTypes);
	if (status < 0) {
		return SOCKET_SEND_FAILED;
	}

	unsigned int size;
	unsigned int type;
	status = binderReceiver.receiveUnsignedInt(size);
	if (status <= 0) {
		debug_message("receive size failed");
		return CANNOT_RECEIVE_FROM_BINDER;
	}
	status = binderReceiver.receiveUnsignedInt(type);
	if (status <= 0) {
		debug_message("receive type failed");
		return CANNOT_RECEIVE_FROM_BINDER;
	}
	if (type == LOC_SUCCESS) {
		assert(size == HOSTNAME_SIZE + PORT_SIZE);
		char buffer[size];
		char *bufferHead = buffer;
		binderReceiver.receiveMessage(size, bufferHead);
		char nameBuffer[HOSTNAME_SIZE];
		bufferHead = extractCharArray(bufferHead, nameBuffer, HOSTNAME_SIZE);
		string serverName(nameBuffer);
		unsigned short port;
		extractUnsignedShort(bufferHead, port);
		int serverSocket;
		status = createSocketAndConnect(serverName.c_str(), to_string(port).c_str(), serverSocket);
		if (status < 0) return status;
		debug_message("loc returned " + serverName + " " + to_string(port));
		status = clientExecute(serverSocket, functionName, argTypes, args);
		close(serverSocket);
		return status;
	} else {
		assert(type == LOC_FAILURE);
		assert(size == 4);
		int reasonCode;
		binderReceiver.receiveInt(reasonCode);
		return reasonCode;
	}
}

int rpcTerminate() {
    int status = 0;
    if (binderSocket == -1) {
        status = connectToBinder();
    }
    if (status == 0) {
        Sender sender(binderSocket);
        debug_message("sending teriminate");
        status = sender.sendTerminate();
        close(binderSocket);
        binderSocket = -1;
    }
    return status;
}

int rpcCacheCall(const char* name, int* argTypes, void** args) {
	return NOT_IMPLEMENTED;
}