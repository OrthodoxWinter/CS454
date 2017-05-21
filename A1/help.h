#ifndef __HELP_H__
#define __HELP_H__ true

#include <sys/socket.h>
#include <string>
#include <iostream>

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

void debug_message(std::string str) {
    #ifndef DEBUG
    std::cout << "DEBUG: " << str << std::endl;
    #endif
}

#endif
