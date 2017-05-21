all: stringClient stringServer

stringClient: client.cc
	g++ client.cc -o stringClient -std=c++11 -lpthread -Wall -Werror

stringServer: server.cc
	g++ server.cc -o stringServer -std=c++11 -Wall -Werror

clean: 
	rm -f stringClient stringServer
