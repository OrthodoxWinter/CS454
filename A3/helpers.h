#ifndef __HELPER_H__
#define __HELPER_H__ 1

//#define DEBUG true

#include <sys/socket.h>
#include <string>
#include <iostream>

#define HOSTNAME_SIZE 1024
#define FUNCTION_NAME_SIZE 64

enum MessageType
{
	ERROR = 0,
	REGISTER = 1,
    REGISTER_SUCCESS = 2,
    REGISTER_FAILURE = 3,
	LOC_REQUEST = 4,
	LOC_SUCCESS = 5,
	LOC_FAILURE = 6,
	EXECUTE = 7,
	EXECUTE_SUCCESS = 8,
	EXECUTE_FAILURE = 9,
    TERMINATE = 10
};

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

void debug(std::string str) {
    #ifdef DEBUG
    std::cout << "DEBUG: " << str << std::endl;
    #endif
}

#endif