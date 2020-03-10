// PROGRAMA p8.c
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *envp[])
{
    pid_t pid;
    int status;

    if (argc != 2) {
        printf("usage: %s dirname\n",argv[0]);
        exit(1);
    }

    pid=fork();
    if (pid > 0) {
        printf("My child is going to execute command \"ls -laR %s\"\n", argv[1]);
        wait(&status);
        printf("My son exited with EXIT CODE = %d\n",WEXITSTATUS(status));

        if (WIFEXITED(status))
            printf("Proccess terminated normally.\n");
        else if (WIFSIGNALED(status))
            printf("Proccess was interrupted.\n");
    }
    else if (pid == 0){
        execlp("ls","ls","-laR",argv[1], NULL);
        printf("Command not executed !\n");
        exit(1);
    }
    
    exit(0);
} 