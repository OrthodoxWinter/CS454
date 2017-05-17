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
	terminate_client = true;
}

void send_to_server(int sockfd, string input) {
	const char *cstr = input.c_str();
	unsigned int len = strlen(cstr) + 1;
	unsigned int len_data = htonl(len);
	if (send_all(sockfd, (const char*) &len_data, sizeof len) < 0) {
		perror("can't send string size");
		exit(1);
	}
	if (send_all(sockfd, cstr, len) < 0) {
		perror("can't send string");
		exit(1);
	}
	cout << "send string " << input << endl;
	sleep(2);
	char *response = new char[len];
	if (recv_all(sockfd, response, len) < 0) {
		perror("can't receive title case string");
		exit(1);
	}
	cout << "Server: " << response << endl;
	delete [] response;
}

void send_request() {
	string request_string;
	while (!terminate_client) {
		{
			unique_lock<mutex> lk(mtx);
			while (input_buffer.empty()) {
				cv.wait(lk);
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

	getaddrinfo("ubuntu", "41716", &hints, &addr_info);
	sockfd = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol);
	if (sockfd < 0) {
		perror("error creating socket");
		exit(1);
	}

	if (connect(sockfd, addr_info->ai_addr, addr_info->ai_addrlen) < 0) {
		close(sockfd);
		perror("error connecting");
		exit(2);
	}

	cout << "successfully connected" << endl;

	thread input (read_inputs);
	thread execute (send_request);

	input.join();
	execute.join();

	close(sockfd);

	return 0;
}
