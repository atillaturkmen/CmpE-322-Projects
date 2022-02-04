/**
 * @file part_c_logger_client.c
 * @author Atilla Türkmen
 *
 * @brief Client of logger program in part c. Sends two numbers and one number or char to server
 *
 * This code is taken and modified from https://www.geeksforgeeks.org/socket-programming-cc/
 *
 * Program takes five arguments: server_ip server_port num1 num2 output
 * Opens a connection to given port and sends message to server.
 */

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
	// five arguments: server_ip server_port num1 num2 output
	if (argc < 6) {
		printf ("usage: %s IP PORT num_1 num_2 output\n", argv[0]);
		return 1;
	}
	const char* IP = argv[1];
	int PORT = atoi(argv[2]);
	const char* num_1 = argv[3];
	const char* num_2 = argv[4];
	const char* output = argv[5];

	int sock = 0; // socket file descriptor
	struct sockaddr_in serv_addr;

	// Creating socket file descriptor
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// turn "localhost" to IPv4 address
	if (strcmp(IP, "localhost") == 0) {
		IP = "127.0.0.1";
	}

	// Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, IP, &serv_addr.sin_addr)<=0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	// connect the server
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		return -1;
	}
	// write three arguments to one string
	char message[1000000] = {0};
	sprintf(message, "%s %s %s", num_1, num_2, output);
	// send message to server
	send(sock, message, strlen(message), 0);
	return 0;
}
