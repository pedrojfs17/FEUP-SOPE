#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#define READ 0
#define WRITE 1

int main(void) {
    int fd[2];
    pid_t pid;

    pipe(fd);
    pid = fork();
    if (pid >0) { // pai
        char a[2][10];
        printf("PARENT:\n");
        printf("x y ? "); scanf("%s %s",a[0],a[1]);
        close(fd[READ]);
        write(fd[WRITE],a,2*sizeof(char*));
        close(fd[WRITE]);
    }
    else { //filho
        char b[2][10];
        //printf("SON:\n");
        close(fd[WRITE]);
        read(fd[READ],b,2*sizeof(char*));
        printf("SON:\n"); //WHY HERE AND NOT ABOVE ...?!
        printf("x + y = %d\n", atoi(b[0])+atoi(b[1]));
        printf("x * y = %d\n", atoi(b[0])*atoi(b[1]));
        printf("x - y = %d\n", atoi(b[0])-atoi(b[1]));
        if (atoi(b[1]) != 0) printf("x / y = %f\n", atof(b[0])/atof(b[1]));
        else printf("x / y is not valid. y can not be 0.\n");
        close(fd[READ]);
    }
    
    return 0;
} 