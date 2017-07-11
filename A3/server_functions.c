#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>

/* return: OUT; a, b: IN */
int f0(int a, int b) {

  return a + b;
}   


/* returns: OUT; a, b, c, d: IN */
long f1(char a, short b, int c, long d) {

  return a + b * c - d;
}


/* return string is the concatenation of the integer 
   part of the float and the interger part of the double
   return: OUT string; a, b: IN */

char* f2(float a, double b) {

  float ai;
  double bi;
  char *str1;

  a = modff(a, &ai);
  b = modf(b, &bi);

  str1 = (char *)malloc(100);

  sprintf(str1, "%lld%lld", (long long)ai, (long long)bi);

  return str1;
}


/* 
 * bubble sort
 * the first element in the array indicates the size of the array
 * a: INOUT array 
 */

void f3(long a[]) {

  int len = a[0];
  int i, j, k;

  for (i = 0; i < len; i++) {
    for (j = len - 1; j > i; j--) {
      if (a[j] > a[j - 1]) {
	k = a[j];
	a[j] = a[j - 1];
	a[j - 1] = k;
      }
    }
  }
}

/*
 * print file named by a 
 * a: IN array
 */

void f4(char a[]) {

  /* print file a to a printer */
}

double* f11(double a[], double b[], int len) {
  double *returnValue = new double[len];
  for (int i = 0; i < len; i++) {
    returnValue[i] = a[i] * b[i];
  }
  return returnValue;
}

float* f12(float a[], float b[], int len) {
  float *returnValue = new float[len];
  for (int i = 0; i < len; i++) {
    returnValue[i] = a[i] / b[i];
  }
  return returnValue;
}

double f13(int a, char b, short c, float d, double e, long f, int g[], char h[], short i[], float j[], double k[], long l[], int len[]) {
  std::string m(h);
  int n = std::stoi(m);
  int o = b - 48;

  double returnValue = 0;
  for (int i = 0; i < len[0]; i++) {
    returnValue += a * g[i];
  }
  returnValue += n * o;
  for (int j = 0; j < len[1]; j++) {
    returnValue += c * i[j];
  }
  for (int i = 0; i < len[2]; i++) {
    returnValue += d * j[i];
  }
  for (int i = 0; i < len[3]; i++) {
    returnValue += e * k[i];
  }
  for (int i = 0; i < len[4]; i++) {
    returnValue += f * l[i];
  }
  printf("ACTUAL return of f13 is: %f\n", returnValue);
  return returnValue;
}

void longRunning() {
  sleep(10);
}

double f0(double a, int b) {
  return a + b;
}