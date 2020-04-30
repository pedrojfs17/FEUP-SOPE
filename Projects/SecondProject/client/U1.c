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

#include "../server/defs.h"
#include "../utils/logs.h"
#include "../utils/utils.h"

#define MAX_MSG_LEN 255
#define NUM_THREADS_MAX 100000

int i=1;
int closed=0;

void *threader(void * arg){
    
    char *fifo=arg;
    int fd=open(fifo,O_WRONLY);
    
    char msg[MAX_MSG_LEN];
    time_t t;

    int duration = rand() % 20 + 1;

    sprintf(msg,"[ %d, %d, %ld, %d, -1]",i,(int)getpid(),(long)pthread_self(),duration);

    if(fd==-1){
        closed=1;
        writeRegister(i,getpid(),pthread_self(),-1,-1,CLOSED);
        printf("Oops !!! Service is closed !!!\n");
        return NULL;
    }

    writeRegister(i, getpid(), pthread_self(), duration, -1, IWANT);
    write(fd,&msg,MAX_MSG_LEN);
    close(fd);

    char private_fifo[MAX_NAME_LEN]="/tmp/";
    char buff[MAX_MSG_LEN];
    sprintf(buff,"%d",getpid());
    strcat(private_fifo,buff);
    strcat(private_fifo,".");
    sprintf(buff,"%ld",pthread_self());
    strcat(private_fifo,buff);

    int fd_dummy;
    if (mkfifo(private_fifo,0660)<0)
        if (errno == EEXIST) printf("FIFO '%s' already exists\n",private_fifo);
        else{ printf("Can't create FIFO\n"); exit(1);}
    
    if ((fd_dummy=open(private_fifo,O_RDONLY)) < 0){
        printf("Error opening FIFO '%s' in READONLY mode\n",private_fifo);
        exit(2);
    }

    char server_msg[MAX_MSG_LEN];

    if(read(fd_dummy,&server_msg,MAX_MSG_LEN)<0){
        writeRegister(i,getpid(),pthread_self(),duration,-1,FAILED);
        return NULL;
    }
    
    int num1, pid, place;
    long tid;
    sscanf(server_msg,"[ %d, %d, %ld, %d, %d]",&num1,&pid,&tid,&duration, &place);
    if(place==-1 && duration==-1)
        writeRegister(num1,pid,tid,-1,-1,CLOSED);
    else
        writeRegister(num1, pid, tid, duration, place, IAMIN);
        
    close(fd_dummy);

    if (unlink(private_fifo)<0)
        printf("Error when destroying FIFO '%s'\n",private_fifo);

    return NULL;
}

int main(int argc, char *argv[]){
    
    pthread_t threads[NUM_THREADS_MAX];
    cl_args args;
    int t=0;        

    srand(time(NULL));

    if (check_client_arg(&args,argc,argv)==-1) {
        printf("Usage: U1 <-t secs> fifoname\n");
        exit(1);
    }

    initClock();

    printf("Time of execution: %d\tFifoname:%s\n",args.nsecs,args.fifoname);

    
    /*double nsecs;
    sscanf(argv[2],"%lf",&nsecs);*/

    char fifo_copy[MAX_MSG_LEN]="../server/";
    strcat(fifo_copy,args.fifoname);
    
    while(elapsed_time()<args.nsecs && !closed){
        pthread_create(&threads[t],NULL,threader,&fifo_copy);
        pthread_detach(threads[t]);
        //pthread_join(threads[t],NULL);
        usleep(5000);
        i++;
        t++;

    }
    printf("FInished work\n");

    pthread_exit(0);
} 