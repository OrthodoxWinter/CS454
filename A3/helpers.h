#ifndef __HELPER_H__
#define __HELPER_H__ 1

//#define DEBUG true

#include <string>
#include <iostream>
#include <stdlih>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdh>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define HOSTNAME_SIZE 1024
#define FUNCTION_NAME_SIZE 64

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
	SCOKET_CREATE_FAIL = -1,
	SOCKET_CONNECT_FAIL = -2,
	SOCKET_BIND_FAIL = -3,
	SOCKET_LISTEN_FAIL = -4,
	GET_SOCKNAME_FAIL = -5,
	NO_BINDER_ADDRESS_ENV = -6,
	NO_UNSET_BINDER_PORT_ENV = -7
}

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

unsigned int getArgTypesLength(int *argTypes) {
	unsigned int i = 0;
	for (;;) {
		if (argTypes[i] == 0) {
			return i + 1;
		}
	}
	return 0;
}

void debug_message(std::string str) {
    #ifdef DEBUG
    std::cout << "DEBUG: " << str << std::endl;
    #endif
}

int createSocketAndConnect(const char *hostname, const char *port, int &newSocket) {
	struct addrinfo hints, *addr_info;
	memset(&hints, 0, sizeof hints);

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(hostname, port, &hints, &addr_info);
	int socket = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol);
	if (socket < 0) {
		debug_message("error creating socket");
		return SOCKET_CREATE_FAIL;
	}

	if (connect(socket, addr_info->ai_addr, addr_info->ai_addrlen) < 0) {
		close(socket);
		debug_message("error connecting");
		return SOCKET_CONNECT_FAIL;
	}
	newSocket = socket;
	return 0;
}

int createSocketAndListen(std::string &localHostName, unsigned short &localPort, int &localSocket) {
	struct sockaddr_in server_addr;
	socklen_t server_addr_len = sizeof server_addr;
	int listener = socket(AF_INET , SOCK_STREAM , 0);
	if (listener < 0) {
		debug_message("can't create socket");
		return SOCKET_CREATE_FAIL;
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(0);
	memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);
	if (bind(listener, (struct sockaddr*) &server_addr, server_addr_len) < 0) {
		debug_message("can't bind");
		return SOCKET_BIND_FAIL;
	}
	if (listen(listener, 5) < 0) {
		debug_message("can't listen");
		close(listener);
		return SOCKET_LISTEN_FAIL;
	}
	if (getsockname(listener, (struct sockaddr*) &server_addr, &server_addr_len) < 0) {
		debug_message("can't get sockname");
		return GET_SOCKNAME_FAIL;
	}

	char hostname_buf[1024];
	hostname_buf[1023] = '\0';
	gethostname(hostname_buf, 1023);
	std::string hostname(hostname_buf);

	localHostName = hostname;
	localPort = ntohs(server_addr.sin_port);
	localSocket = listener;
	return 0;
}

int accept_new_client(int socket, fd_set *master_fds, set<int> &all_sockets) {
	struct sockaddr_storage remoteaddr;
	socklen_t addrlen = sizeof remoteaddr;
	int new_client_socket = accept(socket, (struct sockaddr*) &remoteaddr, &addrlen);
	if (new_client_socket < 0) {
		debug_message("can't accept");
		exit(1);
	}
	FD_SET(new_client_socket, master_fds);
	all_sockets.insert(new_client_socket);
	debug_message("got new socket " + to_string(new_client_socket));
	return new_client_socket;
}

unsigned int getArrayLength(int argType) {
	return argType & 0x0000FFFF;
}

unsigned int getType(int argType) {
	return argType & 0x00FF0000;
}

int extractArguments(char * buffer, int *argTypes, void **args, unsigned int argTypesLength, bool allocateMemory)
{
    buffer = extractIntArray(buffer, argTypes, argTypesLength);

    for(unsigned int i = 0; i < argTypesLength - 1; i++)
    {
        int argType = argTypes[i];
        unsigned short int length = getArrayLengthFromArgumentType(argType);
        int type = getTypeFromArgumentType(argType);

        if(allocateMemory)
        {
            switch(type)
            {
                case ARG_CHAR:
                {
                    if(length == 0)
                    {

                        char * c = new char();
                        buffer = extractChar(buffer, *c);

                        args[i] = (void *)c;
                    }
                    else
                    {
                        char * cArray = new char[length];
                        buffer = extractCharArray(buffer, cArray, length);
                        args[i] = (void *)cArray;
                    }

                }
                break;
                case ARG_SHORT:
                {
                    if(length == 0)
                    {
                        short * s = new short();
                        buffer = extractShort(buffer, *s);

                        args[i] = (void *)s;
                    }
                    else
                    {
                        short * sArray = new short[length];
                        buffer = extractShortArray(buffer, sArray, length);
                        args[i] = (void *)sArray;
                    }
                }
                break;
                case ARG_INT:
                {
                    if(length == 0)
                    {
                        int * c = new int();
                        buffer = extractInt(buffer, *c);

                        args[i] = (void *)c;
                    }
                    else
                    {
                        int * cs = new int[length];
                        buffer = extractIntArray(buffer, cs, length);
                        args[i] = (void *)cs;
                    }

                }
                break;
                case ARG_LONG:
                {
                    if(length == 0)
                    {
                        long * l = new long();
                        buffer = extractLong(buffer, *l);

                        args[i] = (void *)l;
                    }
                    else
                    {
                        long * lArray = new long[length];
                        buffer = extractLongArray(buffer, lArray, length);
                        args[i] = (void *)lArray;
                    }
                }
                break;
                case ARG_DOUBLE:
                {
                    if(length == 0)
                    {
                        double * d = new double();
                        buffer = extractDouble(buffer, *d);

                        args[i] = (void *)d;
                    }
                    else
                    {
                        double * dArray = new double[length];
                        buffer = extractDoubleArray(buffer, dArray, length);
                        args[i] = (void *)dArray;
                    }
                }
                break;
                case ARG_FLOAT:
                {
                    if(length == 0)
                    {
                        float * f = new float();
                        buffer = extractFloat(buffer, *f);

                        args[i] = (void *)f;
                    }
                    else
                    {
                        float * fArray = new float[length];
                        buffer = extractFloatArray(buffer, fArray, length);
                        args[i] = (void *)fArray;
                    }
                }
                break;
                default:
                break;
            }
        }
        else
        {
             switch(type)
        {
            case ARG_CHAR:
            {
                if(length == 0)
                {
                    buffer = extractChar(buffer, *((char *)args[i]));
                }
                else
                {

                    buffer = extractCharArray(buffer, (char *)args[i], length);
                }

            }
            break;
            case ARG_SHORT:
            {
                if(length == 0)
                {
                    short * s = new short();
                    buffer = extractShort(buffer, *((short *)args[i]));
                }
                else
                {
                    buffer = extractShortArray(buffer, (short *)args[i], length);
                }
            }
            break;
            case ARG_INT:
            {
                if(length == 0)
                {
                    buffer = extractInt(buffer, *((int *)args[i]));
                }
                else
                {
                    buffer = extractIntArray(buffer, (int *)args[i], length);
                }

            }
            break;
            case ARG_LONG:
            {
                if(length == 0)
                {
                    buffer = extractLong(buffer, *((long *)args[i]));
                }
                else
                {
                    buffer = extractLongArray(buffer, (long *)args[i], length);
                }
            }
            break;
            case ARG_DOUBLE:
            {
                if(length == 0)
                {
                    buffer = extractDouble(buffer, *((double *)args[i]));
                }
                else
                {
                    buffer = extractDoubleArray(buffer, (double *)args[i], length);
                }
            }
            break;
            case ARG_FLOAT:
            {
                if(length == 0)
                {
                    buffer = extractFloat(buffer, *((float *)args[i]));
                }
                else
                {
                    buffer = extractFloatArray(buffer, (float *)args[i], length);
                }
            }
            break;
            default:
            break;
        }
        }
    }
    return 0;
}

#endif