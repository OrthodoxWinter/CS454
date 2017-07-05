#include <string>
#include <sys/socket.h>
#include "helpers.h"
#include "buffer.h"
#include "receiver.h"

using namespace std;

Receiver::Receiver(int socket) : socket(socket) {}

int Receiver::receiveUnsignedInt(unsigned int &i) {
    char buffer[4];

    if(receiveMessageGivenSize(4, buffer) == 0)
    {
        RWBuffer b;
        b.extractUnsignedInt(buffer, i);
        return 0;
    }
    else return -1;
}

int Receiver::receiveMessageGivenSize(unsigned int size, char message[]) {
    if (size == 0)  {
        return 0;
    }
    int n = recv_all(socket, message, size)
    if (n == 0) {
        return SOCKET_CONNECTION_FALIURE;
    } else if (n < 0) {
        return SOCKET_RECEIVE_FAILURE;
    } else {
        return 0;
    }
}