This project is about inter-process communication between parent and child process with pipes.

Runs given binary in child process while parent takes inputs from stdin.
Parent creates three pipes. One for sending inputs to child, one for 
stdout of child, one for stderr of child. This way parent can understand if an
error has occured.

Parent takes inputs from stdin. Sends it to child via pipe. Child runs blackbox binary and
attaches its stdin, stdout and sterr to pipes created by parent. Thus anything blackbox returns
is sent to the parent to be logged in a file.
