#include <string>
#include <thread>
#include <mutex>
#include <map>
#include "rpc.h"
#include "binder.h"
#include "structs.h"

using namespace std;

int binderSocket = -1;
int listeningSocket = -1;
string localHostname = "";
unsigned short localPort = 0;
map<function_info, skeleton> functions;

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
	} else {
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
			exit(1);
			return -1;
		}
	}
}

int rpcExecute() {
	
}