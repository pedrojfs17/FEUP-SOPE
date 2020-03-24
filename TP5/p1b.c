#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define READ 0
#define WRITE 1

struct Data {
    int x;
    int y;
};

int main(void) {
    int fd[2];
    pid_t pid;

    pipe(fd);
    pid = fork();
    if (pid >0) { // pai
        struct Data data; 
        printf("PARENT:\n");
        printf("x y ? "); scanf("%d %d",&data.x,&data.y);
        close(fd[READ]);
        write(fd[WRITE],&data,sizeof(data));
        close(fd[WRITE]);
    }
    else { //filho
        struct Data data;
        //printf("SON:\n");
        close(fd[WRITE]);
        read(fd[READ],&data,sizeof(data));
        printf("SON:\n"); //WHY HERE AND NOT ABOVE ...?!
        printf("x + y = %d\n", data.x+data.y);
        printf("x * y = %d\n", data.x*data.y);
        printf("x - y = %d\n", data.x-data.y);
        if (data.y != 0) printf("x / y = %f\n", (float)data.x/(float)data.y);
        else printf("x / y is not valid. y can not be 0.\n");
        close(fd[READ]);
    }
    
    return 0;
} 