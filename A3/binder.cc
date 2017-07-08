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
#include <set>
#include <list>
#include <map>
#include <ctype.h>
#include "buffer.h"
#include "helpers.h"
#include "structs.h"
#include "sender.h"
#include "receiver.h"

using namespace std;

int socket_fd = 0;
list<server_location> serversRoundRobin;
set<int> serverSockets;
map<server_location, list<function_info>> serverFunctions;

void registerServerFunction(string serverName, unsigned short port, string functionName, int *argTypes) {
    bool inQueue = false;
    for (list<server_location>::iterator it = serversRoundRobin.begin(); it != serversRoundRobin.end(); it++) {
        server_location &server = (*it);
        if (server.name == serverName && server.port == port) {
            inQueue = true;
            break;
        }
    }

    server_location newServer;
	newServer.name = serverName;
	newServer.port = port;
    if (!inQueue) {    	
        serversRoundRobin.push_front(newServer);
    }

    function_info newFunction = toFunctionInfo(functionName, argTypes);
    if (serverFunctions.find(newServer) != serverFunctions.end()) {
    	list<function_info> &functions = serverFunctions.find(newServer)->second;
    	for (list<function_info>::iterator it = functions.begin(); it != functions.end(); it++) {
    		if (newFunction == *it) {
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
	for (list<server_location>::iterator it = serversRoundRobin.begin(); it != serversRoundRobin.end(); it++) {
        server_location server = (*it);
        list<function_info> &functions = serverFunctions.at(server);
        for (list<function_info>::iterator j = functions.begin(); j != functions.end(); j++) {
        	function_info &serverFunctionInfo = *j;
        	if (functionInfo == serverFunctionInfo) {
        		serversRoundRobin.erase(it);
        		serversRoundRobin.push_back(server);
        		return server;
        	}
        }
    }
    throw 1;
}

void handleRegister(Sender &sender, char *buffer, unsigned int bufferSize) {
    unsigned short port;
    char * bufferPointer = buffer;

    char nameBuffer[HOSTNAME_SIZE];
    bufferPointer = extractCharArray(bufferPointer, nameBuffer, HOSTNAME_SIZE);
    string serverName = string(nameBuffer);

    bufferPointer = extractUnsignedShort(bufferPointer, port);

    char functionNameBuffer[FUNCTION_NAME_SIZE];
    bufferPointer = extractCharArray(bufferPointer, functionNameBuffer, FUNCTION_NAME_SIZE);
    string functionName = string(functionNameBuffer);

    unsigned int argTypesLength = (bufferSize - HOSTNAME_SIZE - PORT_SIZE - FUNCTION_NAME_SIZE) / 4;

    int argTypes[argTypesLength];
    extractIntArray(bufferPointer, argTypes, argTypesLength);

    registerServerFunction(serverName, port, functionName, argTypes);
    sender.sendRegisterSuccess(0);
}

void handleLoc(Sender &sender, char *buffer, unsigned int bufferSize) {
    char * bufferPointer = buffer;

    char nameBuffer[FUNCTION_NAME_SIZE];
    bufferPointer = extractCharArray(bufferPointer, nameBuffer, FUNCTION_NAME_SIZE);
    string functionName(nameBuffer);

    unsigned int argTypesLength = (bufferSize - FUNCTION_NAME_SIZE) / 4;
    int argTypes[argTypesLength];
    function_info newFunction = toFunctionInfo(functionName, argTypes);

    try {
    	server_location location = getServerLocation(newFunction);
    	sender.sendLocSuccess(location.name, location.port);
    } catch (int e) {
    	sender.sendLocFailure(e);
    }
}

void handleTerminate() {
    for (set<int>::iterator it = serverSockets.begin(); it != serverSockets.end(); it++) {
        Sender sender(*it);
        sender.sendTerminate();
    }
}

int processRequest(int clientSocket, bool &terminate) {
	Receiver receiver(clientSocket);
	Sender sender(clientSocket);

	unsigned int size;
	unsigned int type;
	int status = receiver.receiveUnsignedInt(size);
	if (status < 0) {
		debug_message("can't receive size from client");
		exit(status);
	} else if (status == 0) {
		return -1;
	}
	if (int e = receiver.receiveUnsignedInt(type) <= 0) {
		debug_message("can't receive type from client");
		exit(e);
	}
	if (type == TERMINATE) {
		handleTerminate();
		terminate = true;
	} else {
		char message[size];
		if (int e = receiver.receiveMessage(size, message) <= 0) {
			debug_message("can't receive message from client");
			exit(e);
		}
		if (type == REGISTER) {
			handleRegister(sender, message, size);
		} else if (type == LOC_REQUEST) {
			handleLoc(sender, message, size);
		} else {
			debug_message("unkown message type " + type);
			exit(1);
		}
	}

}

int main(int argc, char *argv[]) {
	struct sockaddr_in server_addr;
	socklen_t server_addr_len = sizeof server_addr;
	int listener = socket(AF_INET , SOCK_STREAM , 0);
	if (listener < 0) {
		debug_message("can't create socket");
		exit(1);
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(0);
	memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);
	if (bind(listener, (struct sockaddr*) &server_addr, server_addr_len) < 0) {
		debug_message("can't bind");
		exit(1);
	}
	if (listen(listener, 5) < 0) {
		debug_message("can't listen");
		close(listener);
		exit(1);
	}
	if (getsockname(listener, (struct sockaddr*) &server_addr, &server_addr_len) < 0) {
		debug_message("can't get sockname");
		exit(1);
	}

	char hostname_buf[HOSTNAME_SIZE];
	hostname_buf[HOSTNAME_SIZE - 1] = '\0';
	gethostname(hostname_buf, HOSTNAME_SIZE - 1);
	string hostname(hostname_buf);

	cout << "SERVER_ADDRESS " << hostname << endl;
	cout << "SERVER_PORT " << ntohs(server_addr.sin_port) << endl;

	fd_set master_fds, read_fds;
	int fdmax;
	FD_SET(listener, &master_fds);
	fdmax = listener;
	set<int> all_sockets;
	all_sockets.insert(listener);

	bool terminate = false;
	while (!terminate) {
		read_fds = master_fds;
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) < 0) {
			exit(1);
		}
		for (int socket : all_sockets) {
			if (FD_ISSET(socket, &read_fds)) {
				if (socket == listener) {
					int new_client_socket = accept_new_client(listener, &master_fds, all_sockets);
					if (new_client_socket > fdmax) {
						fdmax = new_client_socket;
					}
				} else {
					if (processRequest(socket, terminate) == -1) {
						debug_message("closing connection to client");
						FD_CLR(socket, &master_fds);
						all_sockets.erase(socket);
						fdmax = *all_sockets.rbegin();
						close(socket);
					}
					if (terminate) {
						break;
					}
				}
			}
		}
	}
}
