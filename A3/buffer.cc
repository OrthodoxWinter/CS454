#include <stdio.h>
#include "buffer.h"

using namespace std;

char *insertShort(short s, char *buffer) {
	size_t length = sizeof s;
	memcpy(buffer, (const char*) &s, length);
	return buffer + length;
}

char *insertInt(int i, char *buffer) {
	size_t length = sizeof i;
	memcpy(buffer, (const char*) &i, length);
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
	memcpy(buffer, (const char*) &u, length);
	return buffer + length;
}

char *insertChar(char c, char *buffer) {
	buf[0] = c;
	return buffer + 1;
}

char *insertDouble(double d, char *buffer) {
	size_t length = sizeof d;
	memcpy(buffer, (const char*) &d, length);
	return buffer + length;
}

char *insertFloat(float f, char *buffer) {
	size_t length = sizeof f;
	memcpy(buffer, (const char*) &f, length);
	return buffer + length;
}

char *insertLong(long l, char *buffer) {
	size_t length = sizeof l;
	memcpy(buffer, (const char*) &l, length);
	return buffer + length;
}

char *insertUnsignedShort(unsigned short s, char *buffer) {
	size_t length = sizeof s;
	memcpy(buffer, (const char*) &s, length);
	return buffer + length;
}

char *insertIntArray(int *intArray, int length, char *buffer) {
	for (unsigned int i = 0; i < length; i++) {
		buffer = insertInt(intArray[i], bufferHead);
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

char *extractIntArray(char *buffer, int *intArray, unsigned int length);
char *extractShortArray(char *buffer, short *shortArray, unsigned int length); 
char *extractCharArray(char *buffer, char *charArray, unsigned int length); 
char *extractLongArray(char *buffer, long *longArray, unsigned int length); 
char *extractDoubleArray(char *buffer, double *doubleArray, unsigned int length); 
char *extractFloatArray(char *buffer, float *floatArray, unsigned int length); 
char *extractUnsignedInt(char *buffer, unsigned int &i);
char *extractInt(char *buffer, int &i);
char *extractShort(char *buffer, short &i);
char *extractUnsignedShort(char *buffer, unsigned short &us);
char *extractLong(char *buffer, long &l);
char *extractFloat(char *buffer, float &f);
char *extractDouble(char *buffer, double &d);
char *extractChar(char *buffer, char &c);