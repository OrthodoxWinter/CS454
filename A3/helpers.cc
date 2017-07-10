#include <string>
#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <set>
#include "rpc.h"
#include "helpers.h"

#define DEBUG 1;

int send_all(int socket, const char *buf, unsigned int len) {
	unsigned int sent = 0;
	int n;
	while(sent < len) {
		n = send(socket, buf + sent, len - sent, 0);
		if (n <= 0) { 
			break; 
		}
		sent += n;
	}
	return n;
}

int recv_all(int socket, char *buf, unsigned int len) {
	unsigned int received = 0;
	int n;
	while(received < len) {
		n = recv(socket, buf + received, len - received, 0);
		if (n <= 0) { 
			break; 
		}
		received += n;
	}
	return n;
}

unsigned int getArgTypesLength(int *argTypes) {
	unsigned int i = 0;
	for (;;) {
		if (argTypes[i] == 0) {
			return i + 1;
		}
		i++;
	}
	return 0;
}

void debug_message(string str) {
    #ifdef DEBUG
	cout << "DEBUG: " << str << endl;
    #endif
}

int createSocketAndConnect(const char *hostname, const char *port, int &socketReturn) {
	struct addrinfo hints, *addr_info;
	memset(&hints, 0, sizeof hints);

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(hostname, port, &hints, &addr_info);
	int newSocket = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol);
	if (newSocket < 0) {
		debug_message("error creating socket");
		return SOCKET_CREATE_FAIL;
	}

	if (connect(newSocket, addr_info->ai_addr, addr_info->ai_addrlen) < 0) {
		close(newSocket);
		debug_message("error connecting");
		return SOCKET_CONNECT_FAIL;
	}
	socketReturn = newSocket;
	return 0;
}

int createSocketAndListen(string &localHostName, unsigned short &localPort, int &localSocket) {
	struct sockaddr_in server_addr;
	socklen_t server_addr_len = sizeof server_addr;
	int listener = socket(AF_INET , SOCK_STREAM , 0);
	if (listener < 0) {
		debug_message("can't create socket");
		return SOCKET_CREATE_FAIL;
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(0);
	memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);
	if (bind(listener, (struct sockaddr*) &server_addr, server_addr_len) < 0) {
		debug_message("can't bind");
		return SOCKET_BIND_FAIL;
	}
	if (listen(listener, 5) < 0) {
		debug_message("can't listen");
		close(listener);
		return SOCKET_LISTEN_FAIL;
	}
	if (getsockname(listener, (struct sockaddr*) &server_addr, &server_addr_len) < 0) {
		debug_message("can't get sockname");
		return GET_SOCKNAME_FAIL;
	}

	char hostname_buf[1024];
	hostname_buf[1023] = '\0';
	gethostname(hostname_buf, 1023);
	string hostname(hostname_buf);

	localHostName = hostname;
	localPort = ntohs(server_addr.sin_port);
	localSocket = listener;
	return 0;
}

int accept_new_client(int socket, fd_set *master_fds, set<int> &all_sockets) {
	struct sockaddr_storage remoteaddr;
	socklen_t addrlen = sizeof remoteaddr;
	int new_client_socket = accept(socket, (struct sockaddr*) &remoteaddr, &addrlen);
	if (new_client_socket < 0) {
		debug_message("can't accept");
		exit(1);
	}
	FD_SET(new_client_socket, master_fds);
	all_sockets.insert(new_client_socket);
	debug_message("got new socket " + to_string(new_client_socket));
	return new_client_socket;
}

unsigned int getArrayLength(int argType) {
	return argType & 0x0000FFFF;
}

unsigned int getType(int argType) {
	return (argType & 0x00FF0000) >> 16;
}

bool isOutput(int argType) {
	return (argType & 1 << ARG_OUTPUT) != 0;
}

unsigned int getTypeSize(unsigned int type) {
	switch (type) {
		case ARG_CHAR: return 1;
		case ARG_SHORT: return 2;
		case ARG_INT: return 4;
		case ARG_LONG: return 8;
		case ARG_DOUBLE: return 9;
		case ARG_FLOAT: return 4;
		default: 
			debug_message("size unknown for type " + to_string(type));
			exit(1);
	}
}