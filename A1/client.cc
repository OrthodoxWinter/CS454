#include <iostream> 
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include <condition_variable>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include "help.h"

using namespace std;

mutex mtx;
queue<string> input_buffer;
condition_variable cv;
bool terminate_client = false;
int sockfd;

void read_inputs() {
	string input;
	while (getline(cin, input)) {
		lock_guard<mutex> lock(mutex);
		input_buffer.push(input);
		cv.notify_one();
	}

	debug_message("GOT EOF");

	{
		lock_guard<mutex> lock(mutex);
		terminate_client = true;
		cv.notify_one();
	}
}

void send_to_server(int sockfd, string input) {
	debug_message("trying to send send " + input);
	const char *cstr = input.c_str();
	unsigned int len = strlen(cstr) + 1;
	uint32_t len_data = htonl(len);
	if (send_all(sockfd, (const char*) &len_data, sizeof len) < 0) {
		debug_message("can't send string size");
		exit(1);
	}
	if (send_all(sockfd, cstr, len) < 0) {
		debug_message("can't send string");
		exit(1);
	}
	sleep(2);
	char *response = new char[len];
	if (recv_all(sockfd, response, len) < 0) {
		debug_message("can't receive title case string");
		exit(1);
	}
	cout << "Server: " << response << endl;
	delete [] response;
}

void send_request() {
	string request_string;
	for (;;) {
		{
			unique_lock<mutex> lk(mtx);
			if (terminate_client) {
				return;
			}
			while (input_buffer.empty()) {
				cv.wait(lk);
				if (terminate_client) {
					return;
				}
			}
			request_string = input_buffer.front();
			input_buffer.pop();
		}
		send_to_server(sockfd, request_string);
	}
}

int main() {
	struct addrinfo hints, *addr_info;
	memset(&hints, 0, sizeof hints);

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	const char *hostname = getenv("SERVER_ADDRESS");
	const char *port = getenv("SERVER_PORT");
	getaddrinfo(hostname, port, &hints, &addr_info);
	sockfd = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol);
	if (sockfd < 0) {
		debug_message("error creating socket");
		exit(1);
	}

	if (connect(sockfd, addr_info->ai_addr, addr_info->ai_addrlen) < 0) {
		close(sockfd);
		debug_message("error connecting");
		exit(2);
	}

	thread input (read_inputs);
	thread execute (send_request);

	input.join();
	execute.join();

	close(sockfd);

	return 0;
}
