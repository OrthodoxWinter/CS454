#ifndef __SENDER_H__
#define __SENDER_H__ 1

#include <string>
#include <list>

class Sender {
  private:
  	unsigned int executeMessageSize(int *argTypes, unsigned int argTypesLength);
  	int sendExecuteMessage(string name, int *argTypes, void **args, unsigned int type);
  	int sendIntMessage(int i, unsigned int type);
  public:
  	int socket;
	Sender(int socket);
	int sendMessage(unsigned int length, unsigned int messageType, char *message);

	int sendRegister(string serverName, unsigned short port, string functionName, int argTypes);
	int sendRegisterSuccess(int reasonCode);
	int sendRegisterFailure(int reasonCode);

	int sendLoc(string name, int argTypes[]);
	int sendLocSuccess(string serverName, unsigned short port);
	int sendLocFailure(unsigned int unsigned int);

	int sendExecute(string name, int *argTypes, void **args);
	int sendExecuteSuccess(string name, int *argTypes, void **args);
	int sendExecuteFailure(int reasonCode);

	int sendTerminate();
};

#endif