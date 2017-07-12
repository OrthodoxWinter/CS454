#!/bin/bash
SERVER=$1
CLIENT=$2

g++ -c -std=c++11 ${SERVER}.c server_functions.c server_function_skels.c
g++ -L. ${SERVER}.o server_functions.o server_function_skels.o -lrpc -lpthread -o ${SERVER}

g++ -c ${CLIENT}.c
g++ -L. ${CLIENT}.o -lrpc -lpthread -o ${CLIENT}