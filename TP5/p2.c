#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define READ 0
#define WRITE 1

int main(void) {
    int fd[2], fd2[2];
    pid_t pid;

    pipe(fd);
    pipe(fd2);
    pid = fork();
    if (pid >0) { // pai
        int a[2];
        float result;
        printf("PARENT:\n");
        printf("x y ? "); scanf("%d %d",&a[0],&a[1]);
        close(fd[READ]);
        write(fd[WRITE],a,2*sizeof(int));
        close(fd[WRITE]);
        close(fd2[WRITE]);
        //READ
        read(fd2[READ],&result,sizeof(int));
        printf("x + y = %d\n", (int)result);
        read(fd2[READ],&result,sizeof(int));
        printf("x * y = %d\n", (int)result);
        read(fd2[READ],&result,sizeof(int));
        printf("x - y = %d\n", (int)result);
        if (a[1] != 0) {
            read(fd2[READ],&result,sizeof(float));
            printf("x / y = %f\n", result);
        }
        else printf("x / y is not valid. y can not be 0.\n");
        close(fd2[READ]);
    }
    else { //filho
        int b[2];
        float result;
        close(fd[WRITE]);
        read(fd[READ],b,2*sizeof(int));
        printf("SON:\n");
        close(fd[READ]);
        close(fd2[READ]);
        //WRITE
        result = b[0]+b[1];
        write(fd2[WRITE],&result,sizeof(int));
        result = b[0]*b[1];
        write(fd2[WRITE],&result,sizeof(int));
        result = b[0]-b[1];
        write(fd2[WRITE],&result,sizeof(int));
        result = (float)b[0]/(float)b[1];
        write(fd2[WRITE],&result,sizeof(float));
        close(fd2[WRITE]);
    }
    
    return 0;
} 