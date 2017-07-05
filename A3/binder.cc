
#include <string>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <queue>
#include "buffer.h"
#include "helpers.h"
#include "binder.h"

using namespace std;

int socket_fd = 0;
list<server_location> serversRoundRobin;
map<int, server_location> socketToServerName;
map<server_location, list<function_info>> serverFunctions;

function_info toFunctionInfo(string name, int[] argTypes, unsigned int argTypesLength) {
	function_info newFunction;
    newFunction.name = name;
    for (unsigned int i = 0; i < argTypesLength; i++) {
    	newFunction.argTypes.push_back(argTypes[i]);
    }
}

void registerServerFunction(string serverName, unsigned short port, int serverSocket, string functionName, int[] argTypes, unsigned int argTypesLength) {
    bool inQueue = false;
    for (list<server_location*>::iterator it = serversRoundRobin.begin(); it != serversRoundRobin.end(); it++) {
        server_location server = (*it);
        if (server.name == serverName && server.port == port) {
            inQueue = true;
            break;
        }
    }

    if (!inQueue) {
    	server_location newServer;
    	newServer.name = serverName;
    	newServer.port = port;
        serversRoundRobin.push_front(newServer);
        socketToServerName[serverSocket] = newServer;
    }

    function_info newFunction = toFunctionInfo(functionName, argTypes, argTypesLength)
    if (serverFunctions.find(newServer) != serverFunctions.end()) {
    	list<function_info> functions = serverFunctions.find(newServer)->second();
    	for (list<function_info>::iterator it = functions.begin(); it != function.end(); it++) {
    		if (serverFunctionInfoEq(newFunction, *it)) {
    			functions.erase(it);
    			break;
    		}
    	}
    	functions.push_back(newFunction);
    } else {
    	list<function_info> functions;
    	functions.push_back(newFunction);
    	serverFunctions[newServer] = functions;
    }
}

server_location getServerLocation(function_info &functionInfo) {
	for (list<server_location*>::iterator it = serversRoundRobin.begin(); it != serversRoundRobin.end(); it++) {
        server_location server = (*it);
        
    }
}

void handleRegister(Sender &sender, char buffer[], unsigned int size, int serverSocket) {
    unsigned short port;
    char * bufferPointer = buffer;

    char nameBuffer[HOSTNAME_SIZE];
    bufferPointer = extractCharArray(bufferPointer, nameBuffer, HOSTNAME_SIZE);
    string serverName = string(nameBuffer);

    bufferPointer = extractUnsignedShort(bufferPointer, port);

    char functionNameBuffer[FUNCTION_NAME_SIZE];
    bufferPointer = extractCharArray(bufferPointer, functionNameBuffer);
    string functionName = string(functionNameBuffer);

    unsigned int argTypesLength = (bufferSize - HOSTNAME_SIZE - portSize - FUNCTION_NAME_SIZE) / 4;

    int argTypes[argTypesLength];
    extractIntArray(bufferPointer, argTypes, argTypesLength);

    registerServerFunction(serverName, port, serverSocket, functionName, argTypes, argTypesLength);

    //TODO reply to server
}

void handleLocRequest(Sender &sender, char buffer[], unsigned int bufferSize) {
    char * bufferPointer = buffer;

    char nameBuffer[FUNCTION_NAME_SIZE];
    bufferPointer = extractCharArray(bufferPointer, nameBuffer, FUNCTION_NAME_SIZE);
    string name(nameBuffer);

    unsigned int argTypesLength = (bufferSize - FUNCTION_NAME_SIZE) / 4;
    int argTypes[argTypesLength];
    function_info newFunction = toFunctionInfo(functionName, argTypes, argTypesLength)

    server_location location = getServerLocation(key);

    //TODO reply to client
}