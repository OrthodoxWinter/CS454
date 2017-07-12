CXX = g++ -std=c++11 -lpthread -Wall -Wextra -MMD

.PHONY : all clean

OBJECTS_RPC = buffer.o helpers.o receiver.o sender.o rpc.o
OBJECTS_BINDER = binder.o buffer.o helpers.o receiver.o sender.o
DEPENDS = ${OBJECTS:.o=.d}

all : binder rpc

rpc : ${OBJECTS_RPC}
	${CXX} -c buffer.cc helpers.cc receiver.cc sender.cc rpc.cc
	ar rcs librpc.a buffer.o helpers.o receiver.o sender.o rpc.o

binder : ${OBJECTS_BINDER}
	${CXX} -c binder.cc buffer.cc helpers.cc receiver.cc sender.cc
	${CXX} $^ -o $@

-include ${DEPENDS}

clean :
	rm -f *.d *.o binder librpc.a