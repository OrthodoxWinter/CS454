
#ifndef __RECEIVER_H__
#define __RECEIVER_H__ 1

class Receiver {
  public:
  	int socket;
	Receiver(int socket);
	//returns 0 for socket closed, negative number for failure, or size
	int receiveMessage(unsigned int size, char *message);
	int receiveUnsignedInt(unsigned int &i);
	int receiveInt(int &i);
};

#endif