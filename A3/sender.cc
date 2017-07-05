#include <stdio.h>
#include <string.h>
#include <string>
#include "helpers.h"
#include "sender.h"
#include "buffer.h"

using namespace std;

int Sender::sendMessage(unsigned int length, unsigned int messageType, char *message) {
	char buffer[length + 8];
	char *bufferHead = buffer;

	bufferHead = insertUnsignedInt(size, bufferHead);
 	bufferHead = insertUnsignedInt(messageType, bufferHead);

 	memcpy(bufferHead, message, length);

 	return send_all(socket, buffer, size + 8);
}

Sender::Sender(int socket) : socket(socket) {}

int Sender::sendRegister(string serverName, unsigned short port, string functionName, int argTypes[]) {
	unsigned int argTypesLength = argTypesLength(argTypes);

 	unsigned int size = HOSTNAME_SIZE + PORT_SIZE + FUNCTION_NAME_SIZE + argTypesLength * 4;
 	char buffer[size];
 	char *bufferHead = buffer;

 	bufferHead = insertStringToBuffer(serverID, bufferHead, HOSTNAME_SIZE - (serverName.length() + 1));
 	bufferHead = insertShortToBuffer(port, bufferHead);
 	bufferHead = insertStringToBuffer(name, bufferHead, FUNCTION_NAME_SIZE - (serverName.length() + 1));
 	bufferHead = insertIntArrayToBuffer(argTypes, argTypesLength, bufferHead);

 	return sendMessage(size, REGISTER, buffer);
}

int Sender::sendRegisterSuccess(unsigned int reasonCode) {
	char buffer[4];
	insertIntToBuffer(reasonCode, buffer);

 	return sendMessage(4, REGISTER_SUCCESS, buffer);
}

int Sender::sendRegisterFailure(unsigned int reasonCode) {
	char buffer[4];
	insertIntToBuffer(reasonCode, buffer);

 	return sendMessage(size, REGISTER_FAILURE, buffer);
}

int Sender::sendLocRequest(string functionName, int argTypes[]) {
	unsigned int argTypesLength = argTypesLength(argTypes);

	unsigned int size = FUNCTION_NAME_SIZE + argTypesLength * 4;
	char buffer[size];
	char *bufferHead = buffer;
	
	bufferHead = insertStringToBuffer(name, bufferHead, FUNCTION_NAME_SIZE - (functionName.length() + 1));
 	bufferHead = insertIntArrayToBuffer(argTypes, argTypesLength, bufferHead);

 	return sendMessage(size, LOC_REQUEST, buffer);
}

int Sender::sendLocSuccess(string serverName, unsigned short port) {
	unsigned int size = HOSTNAME_SIZE + PORT_SIZE;
	char buffer[size];
	char *bufferHead = buffer;
	
	bufferHead = insertStringToBuffer(serverName, bufferHead, FUNCTION_NAME_SIZE - (functionName.length() + 1));
 	bufferHead = insertShortToBuffer(port, bufferHead);

 	return sendMessage(size, LOC_SUCCESS, buffer);

}

int Sender::sendLocFailure(unsigned int reasonCode) {
	char buffer[4];
	bufferHead = insertIntToBuffer(reasonCode, buffer);

 	return sendMessage(size, LOC_FAILURE, buffer);
}

int Sender::sendTerminate() {
 	return sendMessage(0, TERMINATE, NULL);
}