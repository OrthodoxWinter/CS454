CXX = g++ -std=c++11 -lpthread -Wall -Wextra -MMD

.PHONY : all clean

all : binder
	${CXX} -c rpc.cc
	ar rcs librpc.a buffer.o helpers.o receiver.o sender.o rpc.o

binder : binder.o buffer.o helpers.o receiver.o sender.o
	${CXX} -c binder.cc buffer.cc helpers.cc receiver.cc sender.cc
	${CXX} $^ -o $@

clean :
	rm -f *.d *.o binder librpc.a