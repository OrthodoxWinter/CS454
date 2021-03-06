#ifndef __BUFFER_H__
#define __BUFFER_H__ 1

#include <string>

using namespace std;

char *insertShort(short s, char *buffer);
char *insertInt(int i, char *buffer);
char *insertString(string s, char *buffer, unsigned int padding = 0);
char *insertUnsignedInt(unsigned int u, char *buffer);
char *insertChar(char c, char *buffer);
char *insertDouble(double d, char *buffer);
char *insertFloat(float f, char *buffer);
char *insertLong(long l, char *buffer);
char *insertUnsignedShort(unsigned short us, char *buffer);
char *insertCharArray(char *charArray, unsigned int length, char *buffer);
char *insertIntArray(int *intArray, unsigned int length, char *buffer);
char *insertDoubleArray(double *doubleArray, unsigned int length, char *buffer);
char *insertFloatArray(float *floatArray, unsigned int length, char *buffer) ;
char *insertShortArray(short *shortArray, unsigned int length, char *buffer);
char *insertLongArray(long *longArray, unsigned int length, char *buffer);
char *insertIntoBuffer(string name, int *argTypes, void **args, char *buffer);

char *extractCharArray(char *buffer, char *charArray, unsigned int length);
char *extractIntArray(char *buffer, int *intArray, unsigned int length);
char *extractShortArray(char *buffer, short *shortArray, unsigned int length); 
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
char *extractArguments(char *buffer, int *argTypes, unsigned int argTypesLength, void **args, bool copy);

#endif