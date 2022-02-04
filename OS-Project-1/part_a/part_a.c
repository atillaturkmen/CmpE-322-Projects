/**
 * @file part_a.c
 * @author Atilla Türkmen
 *
 * @brief Runs given binary in child process while parent takes inputs from stdin.
 *
 * Parent creates three pipes. One for sending inputs to child, one for 
 * stdout of child, one for stderr of child. This way parent can understand if an
 * error has occured.
 *
 * Parent takes inputs from stdin. Sends it to child via pipe. Child runs blackbox binary and
 * attaches its stdin, stdout and sterr to pipes created by parent. Thus anything blackbox returns
 * is sent to the parent to be logged in a file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int p2c[2];                          // pipe for parent to child
    int c2p[2];                          // pipe for child stdout to parent
    int c2pError[2];                     // pipe for child stderr to parent
    pid_t pid;                           // for determining who is child and who is parent
    char w_buf[1000000], r_buf[1000000]; // read and write buffers
    char* blackbox = argv[1];
    char* outFileName = argv[2];

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
        close(p2c[0]);         // Parent will not read from p2c
        close(c2p[1]);         // Parent will not write to c2p
        close(c2pError[1]);    // Parent will not write to c2pError

        // read two integers from stdin
        int a, b;
        scanf("%d %d", &a, &b);
        sprintf(w_buf, "%d %d\n", a, b);     // \n at the end is important for child to understand input is over
        write(p2c[1], w_buf, strlen(w_buf)); // send to child

        // read from both pipes
        int noerr = read(c2p[0], r_buf, sizeof(r_buf));
        int err = read(c2pError[0], r_buf, sizeof(r_buf));
        FILE* outFile = fopen(outFileName, "a"); // open output file before writing
        // if no data is sent from pipe attached to stderr of child, blackbox executed with success
        if (err == 0) {
            fprintf(outFile, "SUCCESS:\n%s", r_buf);
        } else {
            fprintf(outFile, "FAIL:\n%s", r_buf);
        }
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

    return (0);
}

