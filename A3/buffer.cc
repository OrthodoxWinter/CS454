#include <string>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <set>
#include <assert.h>
#include <iostream>
#include "rpc.h"
#include "helpers.h"
#include "buffer.h"
#include "helpers.h"

using namespace std;

bool isLittleEndian() {
	return htonl(47) != 47;
}

void swap8(void *v) {
    char in[8], out[8];
    memcpy(in, v, 8);
    out[0] = in[6];
    out[1] = in[7];
    out[2] = in[4];
    out[3] = in[5];
    out[4] = in[2];
    out[5] = in[3];
    out[6] = in[0];
    out[7] = in[1];
    memcpy(v, out, 8);
}

void swap4(void *v) {
	char in[4], out[4];
    memcpy(in, v, 4);
    out[0] = in[2];
    out[1] = in[3];
    out[2] = in[0];
    out[3] = in[1];
    memcpy(v, out, 4);
}

char *insertShort(short s, char *buffer) {
	size_t length = sizeof s;
	assert(length == 2);
	uint16_t n = htons(s);
	memcpy(buffer, (const char*) &n, length);
	return buffer + length;
}

char *insertInt(int i, char *buffer) {
	size_t length = sizeof i;
	assert(length == 4);
	uint32_t n = htonl(i);
	memcpy(buffer, (const char*) &n, length);
	return buffer + length;
}

char *insertString(string s, char *buffer, unsigned int padding) {
	const char *cstr = s.c_str();
	size_t length = s.length() + 1;
	memcpy(buffer, cstr, length);
	for (unsigned int i = 0; i < padding; i++) {
		buffer[length + i] = '\0';
	}
	return buffer + length + padding;
}

char *insertUnsignedInt(unsigned int u, char *buffer) {
	size_t length = sizeof u;
	assert(length == 4);
	uint32_t n = htonl(u);
	memcpy(buffer, (const char*) &n, length);
	return buffer + length;
}

char *insertChar(char c, char *buffer) {
	buffer[0] = c;
	return buffer + 1;
}

char *insertDouble(double d, char *buffer) {
	debug_message("inserting double " + to_string(d));
	size_t length = sizeof d;
	assert(length == 8);
	memcpy(buffer, (const char*) &d, length);
	if (isLittleEndian()) {
		swap8(buffer);
	}
	return buffer + length;
}

char *insertFloat(float f, char *buffer) {
	debug_message("inserting float " + to_string(f));
	size_t length = sizeof f;
	assert(length == 4);
	memcpy(buffer, (const char*) &f, length);
	if (isLittleEndian()) {
		swap4(buffer);
	}
	return buffer + length;
}

char *insertLong(long l, char *buffer) {
	size_t length = sizeof l;
	assert(length == 8);
	memcpy(buffer, (const char*) &l, length);
	if (!isLittleEndian()) {
		swap8(buffer);
	}
	return buffer + length;
}

char *insertUnsignedShort(unsigned short s, char *buffer) {
	size_t length = sizeof s;
	assert(length == 2);
	uint16_t n = htons(s);
	memcpy(buffer, (const char*) &n, length);
	return buffer + length;
}

char *insertCharArray(char *charArray, unsigned int length, char *buffer) {
	for (unsigned int i = 0; i < length; i++) {
		buffer = insertChar(charArray[i], buffer);
	}
	return buffer;
}

char *insertIntArray(int *intArray, unsigned int length, char *buffer) {
	for (unsigned int i = 0; i < length; i++) {
		buffer = insertInt(intArray[i], buffer);
	}
	return buffer;
}

char *insertDoubleArray(double *doubleArray, unsigned int length, char *buffer) {
	for (unsigned int i = 0; i < length; i++) {
		buffer = insertDouble(doubleArray[i], buffer);
	}
	return buffer;
}

char *insertFloatArray(float *floatArray, unsigned int length, char *buffer) {
	for (unsigned int i = 0; i < length; i++) {
		buffer = insertFloat(floatArray[i], buffer);
	}
	return buffer;
}

char *insertShortArray(short *shortArray, unsigned int length, char *buffer) {
	for (unsigned int i = 0; i < length; i++) {
		buffer = insertShort(shortArray[i], buffer);
	}
	return buffer;
}

char *insertLongArray(long *longArray, unsigned int length, char *buffer) {
	for (unsigned int i = 0; i < length; i++) {
		buffer = insertLong(longArray[i], buffer);
	}
	return buffer;
}

char *insertIntoBuffer(string name, int *argTypes, void **args, char *buffer) {
	debug_message("calling insertIntoBuffer");
	unsigned int argTypesLength = getArgTypesLength(argTypes);
	buffer = insertString(name, buffer, FUNCTION_NAME_SIZE - (name.length() + 1));
	buffer = insertUnsignedInt(argTypesLength, buffer);
	buffer = insertIntArray(argTypes, argTypesLength, buffer);

	for(unsigned int i = 0; i < argTypesLength - 1; i++) {
		int argType = argTypes[i];

		unsigned int arrayLength = getArrayLength(argType);
		unsigned int length = arrayLength == 0 ? 1 : arrayLength;

		int type = getType(argType);
		void *arg = args[i];

		debug_message("insert argType " + to_string(argType) + " of length " + to_string(length));

		switch(type) {
			case ARG_CHAR:
				buffer = insertCharArray((char *) arg, length, buffer);
				break;

			case ARG_SHORT:
				buffer = insertShortArray((short *) arg, length, buffer);
				break;

			case ARG_INT: {
				char *head = buffer;
				int array[length];
				buffer = insertIntArray((int *) arg, length, buffer);
				extractIntArray(head, array, length);
				for (unsigned int i = 0; i < length; i++) {
					cout << array[i] << " ";
				}
				cout << endl;
			}
				break;

			case ARG_LONG:
				buffer = insertLongArray((long *) arg, length, buffer);
				break;

			case ARG_DOUBLE:
				buffer = insertDoubleArray((double *) arg, length, buffer);
				break;

			case ARG_FLOAT:
				buffer = insertFloatArray((float *) arg, length, buffer);
				break;

			default:
				debug_message("unknown type " + to_string(type));
				exit(1);
		}
	}
	return buffer;
}

char *extractIntArray(char *buffer, int *intArray, unsigned int length) {
	for (unsigned int i = 0; i < length; i++) {
		buffer = extractInt(buffer, intArray[i]);
	}
	return buffer;
}

char *extractShortArray(char *buffer, short *shortArray, unsigned int length) {
	for (unsigned int i = 0; i < length; i++) {
		buffer = extractShort(buffer, shortArray[i]);
	}
	return buffer;
}

char *extractCharArray(char *buffer, char *charArray, unsigned int length) {
	for (unsigned int i = 0; i < length; i++) {
		buffer = extractChar(buffer, charArray[i]);
	}
	return buffer;
}

char *extractLongArray(char *buffer, long *longArray, unsigned int length) {
	for (unsigned int i = 0; i < length; i++) {
		buffer = extractLong(buffer, longArray[i]);
	}
	return buffer;
}

char *extractDoubleArray(char *buffer, double *doubleArray, unsigned int length) {
	for (unsigned int i = 0; i < length; i++) {
		buffer = extractDouble(buffer, doubleArray[i]);
	}
	return buffer;
}

char *extractFloatArray(char *buffer, float *floatArray, unsigned int length) {
	for (unsigned int i = 0; i < length; i++) {
		buffer = extractFloat(buffer, floatArray[i]);
	}
	return buffer;
}

char *extractUnsignedInt(char *buffer, unsigned int &i) {
	unsigned int n;
	size_t length = sizeof n;
	memcpy((char *) &n, buffer, length);
	i = ntohl(n);
	return buffer + length;
}

char *extractInt(char *buffer, int &i) {
	int n;
	size_t length = sizeof n;
	memcpy((char *) &n, buffer, length);
	i = ntohl(n);
	return buffer + length;
}

char *extractShort(char *buffer, short &s) {
	short n;
	size_t length = sizeof n;
	memcpy((char *) &n, buffer, length);
	s = ntohs(n);
	return buffer + length;
}

char *extractUnsignedShort(char *buffer, unsigned short &s) {
	short n;
	size_t length = sizeof n;
	memcpy((char *) &n, buffer, length);
	s = ntohs(n);
	return buffer + length;
}

char *extractLong(char *buffer, long &l) {
	long n;
	size_t length = sizeof n;
	memcpy((char *) &n, buffer, length);
	if (!isLittleEndian()) {
		swap8((char *) &n);
	}
	l = n;
	return buffer + length;
}

char *extractFloat(char *buffer, float &f) {
	float n;
	size_t length = sizeof n;
	memcpy((char *) &n, buffer, length);
	if (isLittleEndian()) {
		swap4((char *) &n);
	}
	f = n;
	debug_message("extracted float " + to_string(f));
	return buffer + length;
}

char *extractDouble(char *buffer, double &d) {
	double n;
	size_t length = sizeof n;
	memcpy((char *) &n, buffer, length);
	if (isLittleEndian()) {
		swap8((char *) &n);
	}
	d = n;
	debug_message("extracted double " + to_string(d));
	return buffer + length;
}

char *extractChar(char *buffer, char &c) {
	c = buffer[0];
	return buffer + 1;
}

char *extractArguments(char *buffer, int *argTypes, unsigned int argTypesLength, void **args, bool copy) {
	for (unsigned int i = 0; i < argTypesLength - 1; i++) {
		int argType = argTypes[i];
		unsigned int arrayLength = getArrayLength(argType);
		unsigned int type = getType(argType);
		unsigned int size = arrayLength == 0 ? 1 : arrayLength;
		debug_message("extracting argument for argType " + to_string(argType) + " with size " + to_string(size));
		bool output = isOutput(argType);
		if (copy) {
			switch(type) {
				case ARG_CHAR: {
					char *n = new char[size];
					args[i] = (void *) n;
				}
				break;

				case ARG_SHORT: {
					short *n = new short[size];
					args[i] = (void *) n;
				}
				break;

				case ARG_INT: {
					int *n = new int[size];
					args[i] = (void *) n;
				}
				break;

				case ARG_LONG: {
					long *n = new long[size];
					args[i] = (void *) n;
				}
				break;

				case ARG_DOUBLE: {
					double *n = new double[size];
					args[i] = (void *) n;
				}
				break;

				case ARG_FLOAT: {
					float *n = new float[size];
					args[i] = (void *) n;
				}
				break;

				default:
					debug_message("unknown type while extracting arguments " + to_string(type));
					exit(1);
			}
		}
		switch(type) {
			case ARG_CHAR: {
				if (copy || output) {
					buffer = extractCharArray(buffer, (char *) args[i], size);
				} else {
					buffer = buffer + size * getTypeSize(type);
				}
			}
			break;

			case ARG_SHORT: {
				if (copy || output) {
					buffer = extractShortArray(buffer, (short *) args[i], size);
				} else {
					buffer = buffer + size * getTypeSize(type);
				}
			}
			break;

			case ARG_INT: {
				if (copy || output) {
					int *blah = (int *) buffer;
					cout << ntohl(blah[0]) << endl;
					buffer = extractIntArray(buffer, (int *) args[i], size);
					int *array = (int *) args[i];
					for (unsigned int i = 0; i < size; i++) {
						int n = array[i];
						cout << n << " ";
					}
					cout << endl;
				} else {
					buffer = buffer + size * getTypeSize(type);
				}
			}
			break;

			case ARG_LONG: {
				if (copy || output) {
					buffer = extractLongArray(buffer, (long *) args[i], size);
				} else {
					buffer = buffer + size * getTypeSize(type);
				}
			}
			break;

			case ARG_DOUBLE: {
				if (copy || output) {
					buffer = extractDoubleArray(buffer, (double *) args[i], size);
				} else {
					buffer = buffer + size * getTypeSize(type);
				}
			}
			break;

			case ARG_FLOAT: {
				if (copy || output) {
					buffer = extractFloatArray(buffer, (float *) args[i], size);
				} else {
					buffer = buffer + size * getTypeSize(type);
				}
			}
			break;

			default:
				debug_message("unknown argType " + to_string(type));
				exit(1);
		}
	}
	return buffer;
}