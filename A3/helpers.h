#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <string>
#include <set>

#define HOSTNAME_SIZE 64
#define FUNCTION_NAME_SIZE 64
#define PORT_SIZE 2

using namespace std;

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
	SOCKET_CREATE_FAIL = -1,
	SOCKET_CONNECT_FAIL = -2,
	SOCKET_BIND_FAIL = -3,
	SOCKET_LISTEN_FAIL = -4,
	GET_SOCKNAME_FAIL = -5,
	NO_BINDER_ADDRESS_ENV = -6,
	NO_UNSET_BINDER_PORT_ENV = -7,
	RPC_NOT_INIT = -8,
	SOCKET_SEND_FAILED = -9,
	SOCKET_CLOSED = -10,
	UNEXPECTED_MESSAGE = -11,
	TERMINATE_NOT_FROM_BINDER = -12,
	NO_FUNCTIONS_REGISTERED = -13,
	SOCKET_RECEIVE_FAIL = -14
};

int send_all(int socket, const char *buf, unsigned int len);

int recv_all(int socket, char *buf, unsigned int len);

unsigned int getArgTypesLength(int *argTypes);

void debug_message(string str);

int createSocketAndConnect(const char *hostname, const char *port, int &socketReturn);

int createSocketAndListen(string &localHostName, unsigned short &localPort, int &localSocket);

int accept_new_client(int socket, fd_set *master_fds, set<int> &all_sockets);

unsigned int getArrayLength(int argType);

unsigned int getType(int argType);

#endif