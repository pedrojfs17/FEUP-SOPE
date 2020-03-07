#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid;
    int status;

    pid=fork();
    if (pid > 0) { // Parent
        wait(&status);
        printf(" World!\n");
    }
    else { // Son
        printf("Hello");
        exit( getpid() % 10 );
    }

    return 0;
} 