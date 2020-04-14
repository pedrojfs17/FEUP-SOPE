#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define READ 0
#define WRITE 1

int main(int argc, char *argv[]) {
    int lsPipe[2];
    int grepPipe[2];
    char output[4096];
    pid_t pid, pid2;

    if (argc != 3) {printf("usage: %s dir arg\n",argv[0]); exit(1);}

    pipe(lsPipe);
    pipe(grepPipe);
    pid = fork();
    if (pid > 0) { // pai
        dup2(lsPipe[WRITE], STDOUT_FILENO);
        close(lsPipe[READ]);
        execl("/bin/ls","ls",argv[1],"-laR",NULL);
        close(lsPipe[WRITE]);
    }
    else { //filho
        pid2 = fork();
        if (pid2 > 0) { // Filho 1
            printf("SON 1:\n");
            //dup2(grepPipe[WRITE], lsPipe[READ]);
            close(lsPipe[WRITE]);
            read(lsPipe[0], output, sizeof(output));
            //printf("%s",output);

            dup2(grepPipe[WRITE], STDOUT_FILENO);
            execl("/bin/grep","grep",argv[2],output,NULL);

            close(lsPipe[READ]);
            close(grepPipe[READ]);
            close(grepPipe[WRITE]);
        }
        else {
            printf("SON 2:\n");
            close(grepPipe[WRITE]);

            read(grepPipe[0], output, sizeof(output));
            printf("%s",output);            

            close(grepPipe[READ]);
        }
    }

    exit(0);
} 