#ifndef __HELPER_H__
#define __HELPER_H__ 1

//#define DEBUG true

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

#define HOSTNAME_SIZE 1024
#define FUNCTION_NAME_SIZE 64

enum MessageType {
	ERROR = 1,
	REGISTER = 2,
    REGISTER_SUCCESS = 3,
    REGISTER_FAILURE = 4,
	LOC_REQUEST = 5,
	LOC_SUCCESS = 6,
	LOC_FAILURE = 7,
	EXECUTE = 8,
	EXECUTE_SUCCESS = 9,
	EXECUTE_FAILURE = 10,
    TERMINATE = 11
};

enum ReasonCode {
	SCOKET_CREATE_FAIL = -1,
	SOCKET_CONNECT_FAIL = -2,
	SOCKET_BIND_FAIL = -3,
	SOCKET_LISTEN_FAIL = -4,
	GET_SOCKNAME_FAIL = -5,
	NO_BINDER_ADDRESS_ENV = -6,
	NO_UNSET_BINDER_PORT_ENV = -7
}

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

unsigned int argTypesLength(int argTypes[]) {
	unsigned int i = 0;
	for (;;) {
		if (argTypes[i] == 0) {
			return i;
		}
	}
	return 0;
}

void debug_message(std::string str) {
    #ifdef DEBUG
    std::cout << "DEBUG: " << str << std::endl;
    #endif
}

int createSocketAndConnect(const char *hostname, const char *port, int &socket) {
	struct addrinfo hints, *addr_info;
	memset(&hints, 0, sizeof hints);

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(hostname, port, &hints, &addr_info);
	socket = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol);
	if (socket < 0) {
		debug_message("error creating socket");
		return SOCKET_CREATE_FAIL;
	}

	if (connect(socket, addr_info->ai_addr, addr_info->ai_addrlen) < 0) {
		close(socket);
		debug_message("error connecting");
		return SOCKET_CONNECT_FAIL;
	}
	return 0;
}

int createSocketAndListen(std::string &localHostName, unsigned short &localPort) {
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
	std::string hostname(hostname_buf);

	localHostName = hostname;
	localPort = ntohs(server_addr.sin_port);
	return 0;
}

#endif