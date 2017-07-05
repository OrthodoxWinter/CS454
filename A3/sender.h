#ifndef __SENDER_H__
#define __SENDER_H__ 1

#include <string>
#include <list>

class Sender {
  public:
  	int socket;
	Sender(int socket);
	int sendMessage(unsigned int length, unsigned int messageType, char *message);

	int sendRegisterSuccess(unsigned int code);
	int sendRegisterFailure(unsigned int code);
	int sendRegisterMessage(string serverName, unsigned short port, string functionName, int argTypes[]);
	int sendLocRequest(string name, int argTypes[]);
	int sendLocSuccess(string serverName, unsigned short port);
	int sendLocFailure(unsigned int unsigned int);
	int sendTerminate();
};

#endif