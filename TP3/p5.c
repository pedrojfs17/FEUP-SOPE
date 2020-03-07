#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void)
{
    int pid, childPid;

    pid=fork();
    if (pid > 0) {
        printf("Hello");
    }
    else {
        childPid = fork();
        if (childPid > 0) {
            printf(" my");
        }
        else {
            printf(" friends!\n");
            exit(getpid() % 10);
        }
        exit(getpid() % 10);
    }
    
    return 0;
} 