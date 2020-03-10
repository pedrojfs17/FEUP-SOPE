// PROGRAMA p8.c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[], char *envp[])
{
    pid_t pid;
    int pfd;

    if (argc != 2 && argc != 3) {
        printf("usage: %s dirname outfile\n",argv[0]);
        exit(1);
    }

    pid=fork();
    if (pid > 0)
        printf("My child is going to execute command \"ls -laR %s\"\n", argv[1]);
    else if (pid == 0){
        if (argc == 3) {
            pfd = open(argv[2], O_WRONLY | O_CREAT, 0666);
            close(1);//Close stdout
            dup(pfd);
        }
        execlp("ls","ls","-la",argv[1],NULL);
        printf("Command not executed !\n");
        exit(1);
    }
    
    exit(0);
} 