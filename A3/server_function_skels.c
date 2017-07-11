#include "server_functions.h"
#include <stdio.h>
#include <string.h>

int f0_Skel(int *argTypes, void **args) {

  *(int *)args[0] = f0(*(int *)args[1], *(int *)args[2]);
  return 0;
}

int f1_Skel(int *argTypes, void **args) {

  *((long *)*args) = f1( *((char *)(*(args + 1))), 
		        *((short *)(*(args + 2))),
		        *((int *)(*(args + 3))),
		        *((long *)(*(args + 4))) );

  return 0;
}

int f2_Skel(int *argTypes, void **args) {

  /* (char *)*args = f2( *((float *)(*(args + 1))), *((double *)(*(args + 2))) ); */
  *args = f2( *((float *)(*(args + 1))), *((double *)(*(args + 2))) );

  return 0;
}

int f3_Skel(int *argTypes, void **args) {

  f3((long *)(*args));
  return 0;
}

/* 
 * this skeleton doesn't do anything except returns
 * a negative value to mimic an error during the 
 * server function execution, i.e. file not exist
 */
int f4_Skel(int *argTypes, void **args) {

  return -1; /* can not print the file */
}

int f11_Skel(int *argTypes, void **args) {
  args[1] = f11((double *) args[0], (double *) args[2], *((int *) args[3]));
  return 0;
}

int f12_Skel(int *argTypes, void **args) {
  *args = f12((float *) args[1], (float *) args[2], *((int *) args[3]));
  return 0;
}

int f13_Skel(int *argTypes, void **args) {
  *(double *)args[0] = f13(*(int *)args[1], *(char *)args[2], *(short *)args[3], *(float *)args[4], *(double *)args[5], *(long *)args[6], (int *)args[7], (char *)args[8], (short *)args[9], (float *)args[10], (double *)args[11], (long *)args[12], (int *)args[13]);
  return 0;
}

int longRunning_Skel(int *argTypes, void **args) {
  longRunning();
  return 0;
}

int f0_1_Skel(int *argTypes, void **args) {
  *(double *)args[0] = f0(*(double *)args[1], *(int *)args[2]);
  return 0;
}