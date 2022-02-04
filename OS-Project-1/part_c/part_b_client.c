/**
 * @file part_b_client.c
 * @author Atilla Türkmen
 *
 * @brief Takes two integers from stdin, sends them to a remote process and writes the result to a file.
 *
 * Program takes three arguments: blackbox path in server, output file to be written and ip address of server.
 * Reads two integers from stdin, sends them and blackbox path to the server. Reads the result from server
 * and finally writes the result to the file.
 *
 * Made by changing rpcgen client template.
 */

#include "part_b.h"


void
part_b_prog_1(char* blackbox, char* outFilePath, char *host)
{
	CLIENT *clnt;
	char * *result_1;
	params  part_b_1_arg;

#ifndef	DEBUG
	clnt = clnt_create (host, PART_B_PROG, PART_B_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */
	// Read two integers from stdin
	int a, b;
    scanf("%d %d", &a, &b);
    // Put relevant data in a struct to be sent to server
    part_b_1_arg.a = a;
    part_b_1_arg.b = b;
	part_b_1_arg.blackbox = blackbox;
	// send info to server and get result
	result_1 = part_b_1(&part_b_1_arg, clnt);
	if (result_1 == (char **) NULL) {
		clnt_perror (clnt, "call failed");
	}
	else {
		// Write to file if call did not fail
		FILE* outFile = fopen(outFilePath, "a");
		fprintf(outFile, "%s", *result_1);
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;

	// takes three arguments: blackbox path in server, output file to be written and ip address of server
	if (argc < 4) {
		printf ("usage: %s blacbox_path out_file_path server_host\n", argv[0]);
		exit (1);
	}
	host = argv[3];
	part_b_prog_1 (argv[1], argv[2], host);
exit (0);
}
