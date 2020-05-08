#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

#include "../utils/defs.h"
#include "../utils/logs.h"
#include "../utils/utils.h"

#define MAX_MSG_LEN 255
#define NUM_THREADS_MAX 100000

int i=1;
int closed=0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *threader(void * arg){
    
    char *publicFifoName = arg;

    int fd = open(publicFifoName,O_WRONLY|O_NONBLOCK,0660);
    if(fd == -1){
        closed=1;
        writeRegister(i,getpid(),pthread_self(),-1,-1,CLOSED);
        fprintf(stderr, "Oops !!! Service is closed !!!\n");
        pthread_exit(NULL);
    }

    char privateFifoName[MAX_NAME_LEN];
    sprintf(privateFifoName, "/tmp/%d.%ld", getpid(), pthread_self());

    if (mkfifo(privateFifoName,0660)<0){
        if (errno == EEXIST) fprintf(stderr, "FIFO '%s' already exists\n",privateFifoName);
        else { fprintf(stderr, "Can't create FIFO\n"); pthread_exit(NULL);}
    }

    char msg[MAX_MSG_LEN];
    int duration = rand() % 20 + 1;
    pthread_mutex_lock(&mutex);
    int mynum=i;
    i++;
    pthread_mutex_unlock(&mutex);
    sprintf(msg,"[ %d, %d, %ld, %d, -1]",mynum,(int)getpid(),(long)pthread_self(),duration);

    if(write(fd,&msg,MAX_MSG_LEN)<0){
        writeRegister(mynum, getpid(), pthread_self(), -1, -1, FAILED);
        close(fd);
        if (unlink(privateFifoName) < 0) fprintf(stderr, "Cannot delete private FIFO");
        closed = 1;
        pthread_exit(NULL);
    }

    writeRegister(mynum, getpid(), pthread_self(), duration, -1, IWANT);
    close(fd);
    
    int privateFifo;
    if ((privateFifo=open(privateFifoName,O_RDONLY)) <= 0){
        fprintf(stderr, "Error opening FIFO '%s' in READONLY mode\n",privateFifoName);
        if (unlink(privateFifoName) < 0) fprintf(stderr, "Cannot delete private FIFO");
        pthread_exit(NULL);
    }

    char server_msg[MAX_MSG_LEN];
    
    if(read(privateFifo,&server_msg,MAX_MSG_LEN)<0){
        fprintf(stderr, "Can't read from private FIFO\n");
        writeRegister(mynum,getpid(),pthread_self(),duration,-1,FAILED);
        if (close(privateFifo) < 0)
            fprintf(stderr, "Error closing FIFO %s file descriptor.\n", privateFifoName);
        if (unlink(privateFifoName)<0)
            fprintf(stderr, "Error when destroying FIFO '%s'\n",privateFifoName);
        pthread_exit(NULL);
    }
    
    int num1, pid, place;
    long tid;
    sscanf(server_msg,"[ %d, %d, %ld, %d, %d]",&num1,&pid,&tid,&duration, &place);

    if(place == -1 && duration == -1) {
        closed = 1;
        writeRegister(num1,pid,tid,-1,-1,CLOSED);
        fprintf(stderr, "Oops !!! Service is closed !!!\n");
    }
    else
        writeRegister(num1, pid, tid, duration, place, IAMIN);
        
    if (close(privateFifo) < 0)
        fprintf(stderr, "Error closing FIFO %s file descriptor.\n", privateFifoName);

    if (unlink(privateFifoName) < 0)
        fprintf(stderr, "Error when destroying FIFO '%s'\n",privateFifoName);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]){
    
    pthread_t threads[NUM_THREADS_MAX];
    cl_args args;
    int t=0;        

    srand(time(NULL));

    if (check_client_arg(&args,argc,argv)==-1) {
        fprintf(stderr, "Usage: U1 <-t secs> fifoname\n");
        exit(1);
    }

    initClock();

    char publicFifoName[MAX_MSG_LEN]="server/";
    strcat(publicFifoName,args.fifoname);

    fprintf(stderr, "Time of execution: %d\tFifoname:%s\n",args.nsecs,publicFifoName);

    while(elapsed_time()<args.nsecs && !closed){
        pthread_create(&threads[t],NULL,threader,&publicFifoName);
        pthread_detach(threads[t]);
        t++;
        usleep(5000);
    }
    
    fprintf(stderr, "FInished work! Time : %f\n", elapsed_time());

    exit(0);
} 