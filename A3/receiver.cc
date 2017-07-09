#include <string>
#include <sys/socket.h>
#include "helpers.h"
#include "buffer.h"
#include "receiver.h"

using namespace std;

Receiver::Receiver(int socket) : socket(socket) {}

int Receiver::receiveUnsignedInt(unsigned int &i) {
    char buffer[4];
    int status = receiveMessage(4, buffer);
    if (status == 4) {
        extractUnsignedInt(buffer, i);
        return 4;
    } else {
        return status;
    }
}

int Receiver::receiveInt(int &i) {
    char buffer[4];
    int status = receiveMessage(4, buffer);
    if (status == 4) {
        extractInt(buffer, i);
        return 4;
    } else {
        return status;
    }
}

int Receiver::receiveMessage(unsigned int size, char *message) {
    return recv_all(socket, message, size);
}