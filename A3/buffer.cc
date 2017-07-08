#include <stdio.h>
#include "buffer.h"

using namespace std;

bool isLittleEndian() {
	int n = 1;
	*(char *)&n == 1;
}

void swap8(void * v) {
    char in[8], out[8];
    memcpy(in, v, 8);
    out[0] = in[7];
    out[1] = in[6];
    out[2] = in[5];
    out[3] = in[4];
    out[4] = in[3];
    out[5] = in[2];
    out[6] = in[1];
    out[7] = in[0];
    memcpy(v, out, 8);
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
	buf[0] = c;
	return buffer + 1;
}

char *insertDouble(double d, char *buffer) {
	size_t length = sizeof d;
	assert(length == 8);
	memcpy(buffer, (const char*) &d, length);
	if (isLittleEndian()) {
		swap8(buffer);
	}
	return buffer + length;
}

char *insertFloat(float f, char *buffer) {
	size_t length = sizeof f;
	assert(length == 4);
	uint32_t n = ntohl(f);
	memcpy(buffer, (const char*) &n, length);
	return buffer + length;
}

char *insertLong(long l, char *buffer) {
	size_t length = sizeof l;
	assert(length == 8);
	memcpy(buffer, (const char*) &l, length);
	if (isLittleEndian()) {
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

char *insertCharArray(char *charArray, int length, char *buffer) {
	for (unsigned int i = 0; i < length; i++) {
		buffer = insertChar(charArray[i], buffer);
	}
	return buffer;
}

char *insertIntArray(int *intArray, int length, char *buffer) {
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

char *insertShortArray(short *shortArray, int length, char *buffer) {
	for (unsigned int i = 0; i < length; i++) {
		buffer = insertShort(shortArray[i], buffer);
	}
	return buffer;
}

char *insertLongArray(long *longArray, int length, char *buffer) {
	for (unsigned int i = 0; i < length; i++) {
		buffer = insertLong(longArray[i], buffer);
	}
	return buffer;
}

char *insertIntoBuffer(string name, int *argTypes, void **args, char *buffer) {
	unsigned int argTypesLength = getArgTypesLength(argTypes);
	buffer = insertString(name, buffer, FUNCTION_NAME_SIZE - (name.length() + 1));
	buffer = insertUnsignedInt(argTypesLength, buffer);
	buffer = insertIntArray(argTypes, argTypesLength, buffer);

	for(unsigned int i = 0; i < argTypesLength - 1; i++) {
		int argType = argTypes[i];

		unsigned int length = getArrayLength(argType);
		unsigned int size = length == 0 ? 1 : length;

		int type = getType(argType);
		void *arg = args[i];

		switch(type) {
			case ARG_CHAR:
				buffer = insertCharArray((char *) arg, buffer);
				break;

			case ARG_SHORT:
				buffer = insertShrotArray((short *) arg, buffer);
				break;

			case ARG_INT:
				buffer = insertIntArray((int *) arg, buffer);
				break;

			case ARG_LONG:
				buffer = insertLongArray((long *) arg, buffer);
				break;

			case ARG_DOUBLE:
				buffer = insertDoubleArary((double *) arg, buffer);
				break;

			case ARG_FLOAT:
				buffer = insertFloatArray((float *) arg, buffer);
				break;

			default:
				return -1;
		}
	}
	return 0;
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
	memcpy((char *) &n, buffer, sizeof length);
	i = n;
	return buffer + length;
}

char *extractInt(char *buffer, int &i) {
	int n;
	size_t length = sizeof n;
	memcpy((char *) &n, buffer, sizeof length);
	i = n;
	return buffer + length;
}

char *extractShort(char *buffer, short &s) {
	short n;
	size_t length = sizeof n;
	memcpy((char *) &n, buffer, sizeof length);
	s = n;
	return buffer + length;
}

char *extractUnsignedShort(char *buffer, unsigned short &s) {
	short n;
	size_t length = sizeof n;
	memcpy((char *) &n, buffer, sizeof length);
	s = n;
	return buffer + length;
}

char *extractLong(char *buffer, long &l) {
	long n;
	size_t length = sizeof n;
	memcpy((char *) &n, buffer, sizeof length);
	l = n;
	return buffer + length;
}

char *extractFloat(char *buffer, float &f) {
	float n;
	size_t length = sizeof n;
	memcpy((char *) &n, buffer, sizeof length);
	f = n;
	return buffer + length;
}

char *extractDouble(char *buffer, double &d) {
	double n;
	size_t length = sizeof n;
	memcpy((char *) &n, buffer, sizeof length);
	d = n;
	return buffer + length;
}

char *extractChar(char *buffer, char &c) {
	c = buffer[0];
	return buffer + 1;
}