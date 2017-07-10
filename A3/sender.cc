#include <stdio.h>
#include <string.h>
#include <string>
#include <bitset>
#include <iostream>
#include "helpers.h"
#include "sender.h"
#include "buffer.h"

using namespace std;

unsigned int Sender::executeMessageSize(int *argTypes, unsigned int argTypesLength) {
	unsigned int total = 0;
	for (unsigned int i = 0; i < argTypesLength - 1;  i++) {
		int argType = argTypes[i];
		unsigned int type = getType(argType);
		unsigned int arrayLength = getArrayLength(argType);
		arrayLength = arrayLength == 0 ? 1 : arrayLength;
		total += arrayLength * getTypeSize(type);
	}
	return FUNCTION_NAME_SIZE + 4 + argTypesLength * 4 + total;
}

int Sender::sendMessage(unsigned int size, unsigned int messageType, char *message) {
	char buffer[size + 8];
	char *bufferHead = buffer;

	bufferHead = insertUnsignedInt(size, bufferHead);
 	bufferHead = insertUnsignedInt(messageType, bufferHead);

 	memcpy(bufferHead, message, size);

 	return send_all(socket, buffer, size + 8);
}

Sender::Sender(int socket) : socket(socket) {}

int Sender::sendRegister(string serverName, unsigned short port, string functionName, int *argTypes) {
	debug_message("sendRegister with parameters: " + serverName + " " + to_string(port) + " " + functionName);
	unsigned int argTypesLength = getArgTypesLength(argTypes);

 	unsigned int size = HOSTNAME_SIZE + PORT_SIZE + FUNCTION_NAME_SIZE + argTypesLength * 4;
 	char buffer[size];
 	char *bufferHead = buffer;

 	debug_message("insert server name to buffer");
 	bufferHead = insertString(serverName, bufferHead, HOSTNAME_SIZE - (serverName.length() + 1));
 	debug_message("insert port to buffer");
 	bufferHead = insertUnsignedShort(port, bufferHead);
 	debug_message("insert function name to buffer");
 	bufferHead = insertString(functionName, bufferHead, FUNCTION_NAME_SIZE - (functionName.length() + 1));
 	debug_message("insert argTypes to buffer");
 	bufferHead = insertIntArray(argTypes, argTypesLength, bufferHead);

 	debug_message("sending message of size " + to_string(size));
 	return sendMessage(size, REGISTER, buffer);
}

int Sender::sendRegisterSuccess(int reasonCode) {
	return sendIntMessage(reasonCode, REGISTER_SUCCESS);
}

int Sender::sendRegisterFailure(int reasonCode) {
	return sendIntMessage(reasonCode, REGISTER_FAILURE);
}

int Sender::sendLoc(string functionName, int *argTypes) {
	unsigned int argTypesLength = getArgTypesLength(argTypes);

	unsigned int size = FUNCTION_NAME_SIZE + argTypesLength * 4;
	char buffer[size];
	char *bufferHead = buffer;
	
	bufferHead = insertString(functionName, bufferHead, FUNCTION_NAME_SIZE - (functionName.length() + 1));
 	bufferHead = insertIntArray(argTypes, argTypesLength, bufferHead);

 	return sendMessage(size, LOC_REQUEST, buffer);
}

int Sender::sendLocSuccess(string serverName, unsigned short port) {
	unsigned int size = HOSTNAME_SIZE + PORT_SIZE;
	char buffer[size];
	char *bufferHead = buffer;
	
	bufferHead = insertString(serverName, bufferHead, HOSTNAME_SIZE - (serverName.length() + 1));
 	bufferHead = insertUnsignedShort(port, bufferHead);

 	return sendMessage(size, LOC_SUCCESS, buffer);
}

int Sender::sendLocFailure(int reasonCode) {
	return sendIntMessage(reasonCode, LOC_FAILURE);
}

int Sender::sendTerminate() {
 	return sendIntMessage(0, TERMINATE);
}

int Sender::sendExecuteMessage(string name, int *argTypes, void **args, unsigned int type) {
	unsigned int argTypesLength = getArgTypesLength(argTypes);
	unsigned int size = executeMessageSize(argTypes, argTypesLength);
	char buffer[size];
	insertIntoBuffer(name, argTypes, args, buffer);
	debug_message("execute message buffer of size " + to_string(size));
	/*for (unsigned int i = 0; i < size; i++) {
		if (i % 4 == 0) cout << endl;
		cout << bitset<8>(buffer[i]) << " ";
	}
	cout << endl;*/
	return sendMessage(size, type, buffer);
}

int Sender::sendExecute(string name, int *argTypes, void **args) {
	return sendExecuteMessage(name, argTypes, args, EXECUTE);
}
int Sender::sendExecuteSuccess(string name, int *argTypes, void **args) {
	return sendExecuteMessage(name, argTypes, args, EXECUTE_SUCCESS);
}

int Sender::sendExecuteFailure(int reasonCode) {
	return sendIntMessage(reasonCode, EXECUTE_FAILURE);
}

int Sender::sendIntMessage(int i, unsigned int type) {
	char buffer[4];
	insertInt(i, buffer);
 	return sendMessage(4, type, buffer);
}