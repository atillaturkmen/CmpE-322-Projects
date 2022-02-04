/**
 * @file part_c_logger.c
 * @author Atilla Türkmen
 *
 * @brief Server of logger program in part c. Writes incoming messages to a log file
 *
 * This code is taken and modified from https://www.geeksforgeeks.org/socket-programming-cc/
 *
 * Program takes two arguments: log file to write and port
 * Opens a connection to given port and listens incoming messages in a loop.
 * Writes incoming messages to given file.
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

int main(int argc, char const *argv[])
{
	// two arguments: log file to write and port
	if (argc < 3) {
		printf ("usage: %s log_file PORT\n", argv[0]);
		exit (1);
	}
	const char* filePath = argv[1];
	int PORT = atoi(argv[2]);

	int server_fd;  // server socket descriptor
	int new_socket; // used when accepting new messages
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1000000] = {0}; // buffer between connection and file

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Optional, useful when port is in use
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	// Forcefully attaching socket to the given port
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	// Start listening to port
	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	// Accept new messages in infinite loop
	while (1) {
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}
		int valread = read( new_socket , buffer, 1000000); // read from socket to buffer
		FILE* outFile = fopen(filePath, "a"); 		   // open file
		fprintf(outFile, "%s", buffer); 			   // write to file
		fclose(outFile); 							   // close the file
		memset(buffer, 0, sizeof buffer); 			   // clear buffer
	}
	return 0;
}
