all: binder server client

server: server.c server_functions.c server_function_skels.c buffer.cc helpers.cc receiver.cc sender.cc rpc.cc
	g++ server.c server_functions.c server_function_skels.c buffer.cc helpers.cc receiver.cc sender.cc rpc.cc -o server -std=c++11 -lpthread -Wall -Werror

binder: binder.cc buffer.cc helpers.cc receiver.cc sender.cc
	g++ binder.cc buffer.cc helpers.cc receiver.cc sender.cc -o binder -std=c++11 -Wall -Werror

client: client1.c buffer.cc helpers.cc receiver.cc sender.cc rpc.cc
	g++ client1.c buffer.cc helpers.cc receiver.cc sender.cc rpc.cc -o client -std=c++11 -lpthread -Wall -Werror

clean: 
	rm -f binder server client
