#include <string>
#include <sys/socket.h>
#include "helpers.h"
#include "buffer.h"
#include "receiver.h"

using namespace std;

Receiver::Receiver(int socket) : socket(socket) {}

int Receiver::receiveUnsignedInt(unsigned int &i) {
    char buffer[4];
    if (receiveMessage(4, buffer) == 4) {
        extractUnsignedInt(buffer, i);
        return 4;
    }
    else return -1;
}

int Receiver::receiveInt(int &i) {
    char buffer[4];
    if (receiveMessage(4, buffer) == 4) {
        extractInt(buffer, i);
        return 4;
    } else {
        return -1;
    }
}

int Receiver::receiveMessage(unsigned int size, char *message) {
    int n = recv_all(socket, message, size);
    if (n == 0) {
        return SOCKET_CONNECT_FAIL;
    } else if (n < 0) {
        return SOCKET_RECEIVE_FAIL;
    } else {
        return n;
    }
}