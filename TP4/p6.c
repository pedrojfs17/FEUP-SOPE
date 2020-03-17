// PROGRAMA p6.c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>

int delay;

void childhandler(int signo) {
    int status;
    pid_t pid;
    pid = wait(&status);
    printf("Child %d terminated within %d seconds.\n", pid, delay);
    exit(0);
} 

int main(int argc, char *argv[]) {
    pid_t pid;

    signal(SIGCHLD, childhandler); /* quando um processo filho termina */

    pid = fork(); /* envia ao pai o sinal SIGCHLD */
    if (pid == 0) /* filho */
        execvp(argv[2], &argv[2]);
    else { /* pai */
        sscanf(argv[1], "%d", &delay); /* transforma string em valor */ 
        sleep(delay);
        printf("Program %s exceeded limit of %d seconds!\n", argv[2], delay);
        kill(pid, SIGKILL);
    }
}

