#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include "defs.h"
#include "../logs/logs.h"

int place=1;
int occupied=0;
double time_taken = 0.0;


void * func(void * arg){
    int fd_dummy;
    struct cln_msg *cop;
    cop=(struct cln_msg *) arg;

    //printf("Message Received: %s\n",cop->msg);

    char fifo_name[MAX_NAME_LEN]="/tmp/", pidStr[MAX_NAME_LEN], tidStr[MAX_NAME_LEN];
    int i,duration, pid;
    long tid;
    sscanf(cop->msg,"[ %d, %d, %ld, %d, -1]",&i,&pid,&tid,&duration);
    writeRegister(i, pid, tid, duration, -1, RECEIVED);

    sprintf(pidStr, "%d", pid);
    strcat(fifo_name,pidStr);
    strcat(fifo_name,".");
    sprintf(tidStr, "%ld", tid);
    strcat(fifo_name,tidStr);

    while ((fd_dummy=open(fifo_name,O_WRONLY)) < 0) {
        printf("Tryied to open FIFO '%s' and failed. Trying again.\n",fifo_name);
        usleep(1000);
    }
    
    char client_msg[MAX_NAME_LEN];
    sprintf(client_msg,"[ %d, %d, %ld, %d, %d]",i,getpid(),pthread_self(),duration,place++);
    //printf("Sending message: %s\n",client_msg);
    writeRegister(i, getpid(), pthread_self(), duration, place - 1, ENTER);
    write(fd_dummy,&client_msg,MAX_NAME_LEN);
    close(fd_dummy);

    return NULL;
}

int main(int argc, char*argv[]){
    int fd;
    char msg[MAX_NAME_LEN];
    char fifo_cpy[MAX_NAME_LEN];
    struct cln_msg cm;
    pthread_t t;

    if (argc!=4) {
        printf("Usage: U1 <-t secs> fifoname\n");
        exit(1);
    }

    initClock();
    
    strcpy(fifo_cpy,argv[3]);

    if (mkfifo(fifo_cpy,0660)<0) //Makes fifoname
        if (errno == EEXIST) printf("FIFO '%s' already exists\n",fifo_cpy);
        else printf("Can't create FIFO\n");
    else 
        printf("FIFO '%s' sucessfully created\n",fifo_cpy);

    
    if ((fd=open(fifo_cpy,O_RDONLY | O_NONBLOCK)) !=-1)
        printf("FIFO '%s' opened in READONLY mode\n",fifo_cpy);
    else{
        printf("Can't open FIFO\n");
        if (unlink(fifo_cpy)<0)
        printf("Error when destroying FIFO '%s'\n",fifo_cpy);
        else
            printf("FIFO '%s' has been destroyed\n",fifo_cpy);
    }
    
    //struct timeval begin,end;
    //gettimeofday(&begin,NULL); gettimeofday(&end,NULL);
    double nsecs;
    sscanf(argv[2],"%lf",&nsecs);

    do{
        if(read(fd,&msg,MAX_NAME_LEN) > 0 && msg[0] == '['){
            //printf("Read msg: |%s|\n", msg);
            strcpy(cm.msg,msg);
            pthread_create(&t,NULL,func,(void *)&cm);
            pthread_join(t,NULL);
        }
        /*gettimeofday(&end,NULL);
        time_taken=(end.tv_sec-begin.tv_sec)*1e6;
        time_taken=(time_taken+(end.tv_usec-begin.tv_usec))*1e-6;*/
    } while(elapsed_time()<nsecs);

    close(fd);

    printf("CLOSED BATHROOM!\n");


    if (unlink(argv[3])<0)
        printf("Error when destroying FIFO '%s'\n",argv[3]);
    //else
        //printf("FIFO '%s' has been destroyed\n",argv[3]);
    
    
    pthread_exit(0);
} 