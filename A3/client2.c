/*
 * client.c
 * 
 * This file is the client program, 
 * which prepares the arguments, calls "rpcCall", and checks the returns.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>

#include "rpc.h"

using namespace std;

#define CHAR_ARRAY_LENGTH 100

int main() {

  /* prepare the arguments for f0 */
  int a0 = 5;
  int b0 = 10;
  int count0 = 3;
  int return0;
  int argTypes0[count0 + 1];
  void **args0;

  argTypes0[0] = (1 << ARG_OUTPUT) | (ARG_INT << 16);
  argTypes0[1] = (1 << ARG_INPUT) | (ARG_INT << 16);
  argTypes0[2] = (1 << ARG_INPUT) | (ARG_INT << 16);
  argTypes0[3] = 0;
    
  args0 = (void **)malloc(count0 * sizeof(void *));
  args0[0] = (void *)&return0;
  args0[1] = (void *)&a0;
  args0[2] = (void *)&b0;

  /* prepare the arguments for f1 */
  char a1 = 'a';
  short b1 = 100;
  int c1 = 1000;
  long d1 = 10000;
  int count1 = 5;
  long return1;
  int argTypes1[count1 + 1];
  void **args1;
    
  argTypes1[0] = (1 << ARG_OUTPUT) | (ARG_LONG << 16);
  argTypes1[1] = (1 << ARG_INPUT) | (ARG_CHAR << 16);
  argTypes1[2] = (1 << ARG_INPUT) | (ARG_SHORT << 16);
  argTypes1[3] = (1 << ARG_INPUT) | (ARG_INT << 16);
  argTypes1[4] = (1 << ARG_INPUT) | (ARG_LONG << 16);
  argTypes1[5] = 0;
    
  args1 = (void **)malloc(count1 * sizeof(void *));
  args1[0] = (void *)&return1;
  args1[1] = (void *)&a1;
  args1[2] = (void *)&b1;
  args1[3] = (void *)&c1;
  args1[4] = (void *)&d1;
    
  /* prepare the arguments for f2 */
  float a2 = 3.14159;
  double b2 = 1234.1001;
  int count2 = 3;
  char *return2 = (char *)malloc(CHAR_ARRAY_LENGTH * sizeof(char));
  int argTypes2[count2 + 1];
  void **args2;

  argTypes2[0] = (1 << ARG_OUTPUT) | (ARG_CHAR << 16) | CHAR_ARRAY_LENGTH;
  argTypes2[1] = (1 << ARG_INPUT) | (ARG_FLOAT << 16);
  argTypes2[2] = (1 << ARG_INPUT) | (ARG_DOUBLE << 16);
  argTypes2[3] = 0;

  args2 = (void **)malloc(count2 * sizeof(void *));
  args2[0] = (void *)return2;
  args2[1] = (void *)&a2;
  args2[2] = (void *)&b2;

  /* prepare the arguments for f3 */
  long a3[11] = {11, 109, 107, 105, 103, 101, 102, 104, 106, 108, 110};
  int count3 = 1;
  int argTypes3[count3 + 1];
  void **args3;

  argTypes3[0] = (1 << ARG_OUTPUT) | (1 << ARG_INPUT) | (ARG_LONG << 16) | 11;
  argTypes3[1] = 0;

  args3 = (void **)malloc(count3 * sizeof(void *));
  args3[0] = (void *)a3;

  /* prepare the arguemtns for f4 */
  const char *a4 = "non_exist_file_to_be_printed";
  int count4 = 1;
  int argTypes4[count4 + 1];
  void **args4;

  argTypes4[0] = (1 << ARG_INPUT) | (ARG_CHAR << 16) | strlen(a4);
  argTypes4[1] = 0;

  args4 = (void **)malloc(count4 * sizeof(void *));
  args4[0] = (void *)a4;

  double rtn11[3] = {0, 0, 0};
  double a11[3] = {0.1, 0.2, 0.3};
  double b11[3] = {0.3, 0.4, 0.5};
  int c11 = 3;
  int count11 = 4;
  int argTypes11[count11 + 1];
  void **args11 = (void **)malloc(count11 * sizeof(void *));
  args11[1] = (void *) rtn11;
  args11[0] = (void *) a11;
  args11[2] = (void *) b11;
  args11[3] = (void *) &c11;
  argTypes11[1] = (1 << ARG_OUTPUT) | (ARG_DOUBLE << 16) | 3;
  argTypes11[0] = (1 << ARG_INPUT) | (ARG_DOUBLE << 16) | 3;
  argTypes11[2] = (1 << ARG_INPUT) | (ARG_DOUBLE << 16) | 3;
  argTypes11[3] = (1 << ARG_INPUT) | (ARG_INT << 16);
  argTypes11[4] = 0;

  float rtn12[3] = {0, 0, 0};
  float a12[3] = {0.1, 0.2, 0.3};
  float b12[3] = {0.3, 0.4, 0.5};
  int c12 = 3;
  int count12 = 4;
  int argTypes12[count12 + 1];
  void **args12 = (void **)malloc(count12 * sizeof(void *));
  args12[0] = (void *) rtn12;
  args12[1] = (void *) a12;
  args12[2] = (void *) b12;
  args12[3] = (void *) &c12;
  argTypes12[0] = (1 << ARG_OUTPUT) | (ARG_FLOAT << 16) | 3;
  argTypes12[1] = (1 << ARG_INPUT) | (ARG_FLOAT << 16) | 3;
  argTypes12[2] = (1 << ARG_INPUT) | (ARG_FLOAT << 16) | 3;
  argTypes12[3] = (1 << ARG_INPUT) | (ARG_INT << 16);
  argTypes12[4] = 0;

  double rtn13 = 0;
  int a13 = 1;
  char b13 = '2'; 
  short c13 = 3;
  float d13 = 0.4;
  double e13 = 0.5;
  long f13 = 6;
  int g13[2] = {2, 3};
  char h13[3] = {'1', '2', '\0'}; 
  short i13[2] = {5, 7};
  float j13[4] = {0.2, 0.7};
  double k13[3] = {0.33, 4.5};
  long l13[1] = {10};
  int len13[5] = {2, 2, 4, 3, 1};
  int count13 = 14;
  void **args13 = (void **)malloc(count13 * sizeof(void *));
  args13[0] = (void *) &rtn13;
  args13[1] = (void *) &a13;
  args13[2] = (void *) &b13;
  args13[3] = (void *) &c13;
  args13[4] = (void *) &d13;
  args13[5] = (void *) &e13;
  args13[6] = (void *) &f13;
  args13[7] = (void *) g13;
  args13[8] = (void *) h13;
  args13[9] = (void *) i13;
  args13[10] = (void *) j13;
  args13[11] = (void *) k13;
  args13[12] = (void *) l13;
  args13[13] = (void *) len13;
  int argTypes13[count13 + 1];
  argTypes13[0] = (1 << ARG_OUTPUT) | (ARG_DOUBLE << 16);
  argTypes13[1] = (1 << ARG_OUTPUT) | (ARG_INT << 16);
  argTypes13[2] = (1 << ARG_OUTPUT) | (ARG_CHAR << 16);
  argTypes13[3] = (1 << ARG_OUTPUT) | (ARG_SHORT << 16);
  argTypes13[4] = (1 << ARG_OUTPUT) | (ARG_FLOAT << 16);
  argTypes13[5] = (1 << ARG_OUTPUT) | (ARG_DOUBLE << 16);
  argTypes13[6] = (1 << ARG_OUTPUT) | (ARG_LONG << 16);
  argTypes13[7] = (1 << ARG_OUTPUT) | (ARG_INT << 16) | 2;
  argTypes13[8] = (1 << ARG_OUTPUT) | (ARG_CHAR << 16) | 3;
  argTypes13[9] = (1 << ARG_OUTPUT) | (ARG_SHORT << 16) | 2;
  argTypes13[10] = (1 << ARG_OUTPUT) | (ARG_FLOAT << 16) | 4;
  argTypes13[11] = (1 << ARG_OUTPUT) | (ARG_DOUBLE << 16) | 3;
  argTypes13[12] = (1 << ARG_OUTPUT) | (ARG_LONG << 16) | 1;
  argTypes13[13] = (1 << ARG_OUTPUT) | (ARG_INT << 16) | 5;
  argTypes13[14] = 0;

  double a0_1 = 5.7;
  int b0_1 = 10;
  int count0_1 = 3;
  double return0_1;
  int argTypes0_1[count0_1 + 1];
  void **args0_1;

  argTypes0_1[0] = (1 << ARG_OUTPUT) | (ARG_DOUBLE << 16);
  argTypes0_1[1] = (1 << ARG_INPUT) | (ARG_DOUBLE << 16);
  argTypes0_1[2] = (1 << ARG_INPUT) | (ARG_INT << 16);
  argTypes0_1[3] = 0;
    
  args0_1 = (void **)malloc(count0_1 * sizeof(void *));
  args0_1[0] = (void *)&return0_1;
  args0_1[1] = (void *)&a0_1;
  args0_1[2] = (void *)&b0_1;

  int s11 = rpcCall("f11", argTypes11, args11);
  if (s11 >= 0) {
    printf("ACTUAL return of f11 is: ");
    for (int i = 0; i < 3; i++) {
      printf(" %f", *(((double *)args11[0]) + i));
    }
    printf("\n");
  }
  else {
    printf("Error: %d\n", s11);
  }

  s11 = rpcCall("f11", argTypes11, args11);
  if (s11 >= 0) {
    printf("ACTUAL return of f11 is: ");
    for (int i = 0; i < 3; i++) {
      printf(" %f", *(((double *)args11[0]) + i));
    }
    printf("\n");
  }
  else {
    printf("Error: %d\n", s11);
  }

  s11 = rpcCall("f11", argTypes11, args11);
  if (s11 >= 0) {
    printf("ACTUAL return of f11 is: ");
    for (int i = 0; i < 3; i++) {
      printf(" %f", *(((double *)args11[0]) + i));
    }
    printf("\n");
  }
  else {
    printf("Error: %d\n", s11);
  }

  int s4 = rpcCall("f4", argTypes4, args4);
  /* test the return of f4 */
  printf("\ncalling f4 to print an non existed file on the server");
  printf("\nEXPECTED return of f4: some integer other than 0");
  printf("\nACTUAL return of f4: %d\n", s4);

  s11 = rpcCall("f11", argTypes11, args11);
  if (s11 >= 0) {
    printf("ACTUAL return of f11 is: ");
    for (int i = 0; i < 3; i++) {
      printf(" %f", *(((double *)args11[0]) + i));
    }
    printf("\n");
  }
  else {
    printf("Error: %d\n", s11);
  }

  int s12 = rpcCall("f12", argTypes12, args12);
  if (s12 >= 0) {
    printf("ACTUAL return of f12 is: ");
    for (int i = 0; i < 3; i++) {
      printf(" %f", *(((float *)args12[0]) + i));
    }
    printf("\n");
  }
  else {
    printf("Error: %d\n", s12);
  }

  /* rpcTerminate */
  printf("\ndo you want to terminate? y/n: ");
  if (getchar() == 'y')
    rpcTerminate();

  /* end of client.c */
  return 0;
}




