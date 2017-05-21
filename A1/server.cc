#include <set>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "help.h"

using namespace std;

int accept_new_client(int socket, fd_set *master_fds, set<int> &all_sockets) {
	struct sockaddr_storage remoteaddr;
	socklen_t addrlen = sizeof remoteaddr;
	int new_client_socket = accept(socket, (struct sockaddr*) &remoteaddr, &addrlen);
	if (new_client_socket < 0) {
		debug_message("can't accept");
		exit(1);
	}
	FD_SET(new_client_socket, master_fds);
	all_sockets.insert(new_client_socket);
	return new_client_socket;
}

void to_title_case(char *str, unsigned int len) {
	str[0] = toupper(str[0]);
	for (unsigned int i = 1; i < len; i++) {
		char prev_c = str[i - 1];
		if (isspace(prev_c)) {
			str[i] = toupper(str[i]);
		} else {
			str[i] = tolower(str[i]);
		}
	}
}

int process_request(int socket) {
	uint32_t string_size = 0;
	int status = recv_all(socket, (char *) &string_size, sizeof string_size);
	if (status < 0) {
		debug_message("can't receive string size from client");
		exit(1);
	} else if (status == 0) {
		return 0;
	}
	unsigned int buffer_size = ntohl(string_size);
	//debug_message("got size " + to_string(buffer_size));
	char *buffer = new char[buffer_size];
	if (recv_all(socket, buffer, buffer_size) < 0) {
		debug_message("can't receive string from client");
		exit(1);
	}
	string str(buffer);
	cout << str << endl;
	to_title_case(buffer, buffer_size - 1);
	send_all(socket, buffer, buffer_size);
	delete [] buffer;
	return 1;
}

int main(int argc, char *argv[]) {
	struct sockaddr_in server_addr;
	socklen_t server_addr_len = sizeof server_addr;
	int listener = socket(AF_INET , SOCK_STREAM , 0);
	if (listener < 0) {
		debug_message("can't create socket");
		exit(1);
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(41716);
	memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);
	if (bind(listener, (struct sockaddr*) &server_addr, server_addr_len) < 0) {
		debug_message("can't bind");
		exit(1);
	}
	if (listen(listener, 5) < 0) {
		debug_message("can't listen");
		close(listener);
		exit(1);
	}
	if (getsockname(listener, (struct sockaddr*) &server_addr, &server_addr_len) < 0) {
		debug_message("can't get sockname");
		exit(1);
	}

	char hostname_buf[1024];
	hostname_buf[1023] = '\0';
	gethostname(hostname_buf, 1023);
	string hostname(hostname_buf);

	cout << "SERVER_ADDRESS " << hostname << endl;
	cout << "SERVER_PORT " << ntohs(server_addr.sin_port) << endl;

	fd_set master_fds, read_fds;
	int fdmax;
	FD_SET(listener, &master_fds);
	fdmax = listener;
	set<int> all_sockets;
	all_sockets.insert(listener);

	for (;;) {
		read_fds = master_fds;
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) < 0) {
			exit(1);
		}
		for (int socket : all_sockets) {
			if (FD_ISSET(socket, &read_fds)) {
				if (socket == listener) {
					int new_client_socket = accept_new_client(listener, &master_fds, all_sockets);
					if (new_client_socket > fdmax) {
						fdmax = new_client_socket;
					}
				} else {
					if (process_request(socket) == 0) {
						debug_message("closing connection to client");
						FD_CLR(socket, &master_fds);
						all_sockets.erase(socket);
						fdmax = *all_sockets.rbegin();
						close(socket);
					}
				}
			}
		}
	}
}
