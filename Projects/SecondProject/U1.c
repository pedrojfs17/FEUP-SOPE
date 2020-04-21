#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include "defs.h"

#define MAX_MSG_LEN 255
#define NUM_THREADS_MAX 100

int i=0;

void *threader(void * arg){
    int fd=open(*(char**)arg|O_WRONLY);

    if(fd==-1){
        printf("Oops !!! Service is closed !!!\n");
    }
    printf("FIFO '%s' openned in WRITEONLY mode\n",*(char**)arg);

    char msg[MAX_MSG_LEN];
    sprintf(msg,"[ %d, %d, %d, %d, -1]",i,getpid(),pthread_self(),rand()%20+1);
    write(fd,&msg,MAX_MSG_LEN);
    close(fd);

    char* private_fifo="/tmp";
    char buff[MAX_MSG_LEN];
    sprintf(buff,"%d",getpid());
    strcat(private_fifo,buff);
    sprintf(buff,"%d",pthread_self());
    strcat(private_fifo,buff);

    if (mkfifo(private_fifo,0660)<0)
        if (errno == EEXIST) printf("FIFO '%s' already exists\n",private_fifo);
        else printf("Can't create FIFO\n");
    else 
        printf("FIFO '%s' sucessfully created\n",private_fifo);
    
    if ((fd_dummy=open(private_fifo,O_RDONLY)) !=-1)
        printf("FIFO '%s' openned in READONLY mode\n",private_fifo);

    char server_msg[MAX_MSG_LEN];

    read(fd_dummy,&server_msg,MAX_MSG_LEN);
    printf("%s",server_msg);
    close(fd_dummy);

    if (unlink(fifoName)<0)
        printf("Error when destroying FIFO '%s'\n",fifoName);
    else
        printf("FIFO '%s' has been destroyed\n",fifoName);


}

int main(int argc, char *argv[])
{
    int fd, fd_dummy;
    char fifoName[MAX_MSG_LEN];

    if (argc!=4) {
        printf("Usage: U1 <-t secs> fifoname\n");
        exit(1);
    }
    printf("Time of execution: %s\tFifoname:%s\n",argv[2],argv[3]);
    time_t t;
    srand((unsigned)time(&t));

    fd=open(argv[3],O_WRONLY);

    if (fd == -1) {
        printf("Oops !!! Service is closed !!!\n");
        exit(1);
    }

    printf("FIFO 'requests' openned in WRITEONLY mode\n");

    char msg[MAX_MSG_LEN];
    sprintf(fifoName, "fifo_ans_%d",getpid());
    sprintf(msg,"%d:%d:%s",atoi(argv[1]),atoi(argv[2]),fifoName);

    write(fd,&msg,MAX_MSG_LEN);

    close(fd);

    if (mkfifo(fifoName,0660)<0)
        if (errno == EEXIST) printf("FIFO '%s' already exists\n",fifoName);
        else printf("Can't create FIFO\n");
    else 
        printf("FIFO '%s' sucessfully created\n",fifoName);
    
    if ((fd_dummy=open(fifoName,O_RDONLY)) !=-1)
        printf("FIFO '%s' openned in READONLY mode\n",fifoName);

    char server_msg[MAX_MSG_LEN];

    read(fd_dummy,&server_msg,MAX_MSG_LEN);
    printf("%s",server_msg);
    close(fd_dummy);

    if (unlink(fifoName)<0)
        printf("Error when destroying FIFO '%s'\n",fifoName);
    else
        printf("FIFO '%s' has been destroyed\n",fifoName);
    
    */

    return 0;
} 