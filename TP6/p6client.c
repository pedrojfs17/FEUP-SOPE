#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <string.h>

#define MAX_MSG_LEN 20

int main(int argc, char *argv[])
{
    int fd;
    int num[2];
    char fifoName[MAX_MSG_LEN];

    if (argc!=3) {
        printf("Usage: p6client <num1> <num2>\n");
        exit(1);
    }

    fd=open("./fifo_req",O_WRONLY);

    if (fd == -1) {
        printf("Oops !!! Service is closed !!!\n");
        exit(1);
    }

    printf("FIFO 'requests' openned in WRITEONLY mode\n");

    num[0]=atoi(argv[1]);
    write(fd,&num[0],sizeof(int));

    num[1]=atoi(argv[2]);
    write(fd,&num[1],sizeof(int));

    if (num[0]!=0 || num[1]!=0) {
        sprintf(fifoName, "fifo_ans_%d\n",getpid());
        write(fd,&fifoName,MAX_MSG_LEN);
    }

    close(fd);
    return 0;
} 