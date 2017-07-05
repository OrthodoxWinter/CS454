
#ifndef __RECEIVER_H__
#define __RECEIVER_H__ 1

class Receiver {
  public:
  	int socket;
	Receiver(int socket);
	int receiveMessage(unsigned int size, char message[]);
	int receiveUnsignedInt(unsigned int &i);
};

#endif