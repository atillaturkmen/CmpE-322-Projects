/**
 * @file part_b_server.c
 * @author Atilla Türkmen
 *
 * @brief Takes two integers and a blackbox path from client, runs the blackbox in given path with these two integers and sends the result back to client.
 *
 * Server side is similar to part a except this time numbers and blackbox come from a remote client.
 * Parent creates a child a process that runs blackbox.
 * Child sends the result to parent. Parent sends the result to client.
 * There are three pipes similar to part a one for parent to child, one for child error and one for child stdout to parent.
 *
 * Made by changing rpcgen server template.
 */

#include "part_b.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

char **
part_b_1_svc(params *argp, struct svc_req *rqstp)
{
	static char * result;

	/*
	 * insert server code here
	 */
	int p2c[2], c2p[2], c2pError[2];     // pipes for parent child communication
    pid_t pid;                           // for determining who is child and who is parent
    char w_buf[1000000], r_buf[1000000]; // read and write buffers between parent and child
    // clear buffers because server runs continously and buffers has to be empty to carry correct info
    // else old information is left in buffers
    memset(w_buf, 0, sizeof w_buf);
    memset(r_buf, 0, sizeof r_buf);
    char* blackbox = argp->blackbox;

    // parent creates the pipes
    pipe(p2c);
    pipe(c2p);
    pipe(c2pError);

    // fork creates two processes
    // pid is used for determining if this process is child or parent
    if ((pid = fork()) == -1) {
        fprintf(stderr, "fork() failed.\n");
        exit(-1);
    }

    // 0 is returned to the parent from fork()
    if (pid > 0) { // parent
        close(p2c[0]);    // Parent will not read from p2c
        close(c2p[1]);    // Parent will not write to c2p
        close(c2pError[1]);    // Parent will not write to c2pError

        // write two integers to a string and send it to child
        sprintf(w_buf, "%d %d\n", argp->a, argp->b); // \n at the end is important for child to understand input is over
        write(p2c[1], w_buf, strlen(w_buf));

        // read from both normal pipe and error pipe
        int noerr = read(c2p[0], r_buf, sizeof(r_buf));
        int err = read(c2pError[0], r_buf, sizeof(r_buf));
        result = malloc(1000000); // allocate memory for result
        // if no data is sent from pipe attached to stderr of child, blackbox executed with success
        if (err == 0) {
        	sprintf(result, "SUCCESS:\n%s", r_buf);
        } else {
        	sprintf(result, "FAIL:\n%s", r_buf);
        }
        // send result back to client
        return &result;
    } else { // child
        dup2(p2c[0], STDIN_FILENO); // child attaches read end of parent to child pipe to its own stdin
        dup2(c2p[1], STDOUT_FILENO); // attaches write end of child to parent pipe to its stdout
        dup2(c2pError[1], STDERR_FILENO); // attaches write end of another child to parent pipe to its stderr

        // child closes pipes that are not needed
        close(c2p[0]);
        close(c2p[1]);
        close(c2pError[0]);
        close(c2pError[1]);
        close(p2c[0]);
        close(p2c[1]);

        // run blackbox binary
        int code = execl(blackbox, blackbox, NULL);
        // program comes here if blacbox binary could not run
        printf("error when opening blackbox.\n code: %d\n", code);
    }
}
