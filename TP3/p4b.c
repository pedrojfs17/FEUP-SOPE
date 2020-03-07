#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid;

    pid=fork();
    if (pid > 0) { // Parent
        printf("Hello");
    }
    else { // Son
        printf(" World!\n");
        exit( getpid() % 10 );
    }

    return 0;
} 