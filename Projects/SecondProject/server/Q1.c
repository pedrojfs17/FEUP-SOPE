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
#include <pthread.h>
#include "../utils/defs.h"
#include "../utils/logs.h"
#include "../utils/utils.h"

int place=1;
int closed=0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void * func(void * arg){
    
    int privateFifo;
    struct cln_msg *cop;
    cop=(struct cln_msg *) arg;

    char privateFifoName[MAX_NAME_LEN]="/tmp/", pidStr[MAX_NAME_LEN], tidStr[MAX_NAME_LEN];
    int i,duration, pid;
    long tid;
    sscanf(cop->msg,"[ %d, %d, %ld, %d, -1]",&i,&pid,&tid,&duration);
    writeRegister(i, pid, tid, duration, -1, RECEIVED);

    sprintf(pidStr, "%d", pid);
    strcat(privateFifoName,pidStr);
    strcat(privateFifoName,".");
    sprintf(tidStr, "%ld", tid);
    strcat(privateFifoName,tidStr);
    int try=0;
    while ((privateFifo=open(privateFifoName,O_WRONLY|O_NONBLOCK)) <= 0 && try<5) {try++;usleep(500);}
    if(try==5){
        writeRegister(i, pid, tid, duration, -1, GAVEUP);
        pthread_exit(NULL);
    }
    
    pthread_mutex_lock(&mutex);
    int my_place = place;
    place++;
    pthread_mutex_unlock(&mutex);

    char client_msg[MAX_NAME_LEN];

    pthread_mutex_lock(&mutex2);
    if(elapsed_time()+duration*1e-3<cop->bathroom_time || closed){
        pthread_mutex_unlock(&mutex2);
        sprintf(client_msg,"[ %d, %d, %ld, %d, %d]",i,getpid(),pthread_self(),duration,my_place);
        writeRegister(i, getpid(), pthread_self(), duration, my_place, ENTER);
    }
    else{
        pthread_mutex_unlock(&mutex2);
        sprintf(client_msg,"[ %d, %d, %ld, %d, %d]",i,getpid(),pthread_self(),-1,-1);
        writeRegister(i, getpid(), pthread_self(), duration, -1, TOOLATE);
    }

    if(write(privateFifo,&client_msg,MAX_NAME_LEN)<=0){
        writeRegister(i, pid, tid, duration, -1, GAVEUP);
        close(privateFifo);
        pthread_exit(NULL);
    }
    close(privateFifo);

    pthread_mutex_lock(&mutex2);
    if(!closed){
        pthread_mutex_unlock(&mutex2);
        usleep(duration*1000);
        writeRegister(i, getpid(), pthread_self(), duration, my_place, TIMEUP);
    }
        

    pthread_exit(NULL);
}

int main(int argc, char*argv[]){
    int fd;
    char msg[MAX_NAME_LEN];
    char publicFifoName[MAX_NAME_LEN]="server/";
    struct cln_msg cm;
    srv_args args;
    args.nsecs=0;
    
    if(check_server_arg(&args,argc,argv)==-1){
        printf("Usage: Q1 <-t secs> fifoname\n");
        exit(1);
    }

    initClock();
    
    strcat(publicFifoName,args.fifoname);
    printf("Time of execution: %d\tFifoname:%s\n",args.nsecs,publicFifoName);

    if (mkfifo(publicFifoName,0660)<0)
        if (errno == EEXIST) printf("FIFO '%s' already exists\n",publicFifoName);
        else printf("Can't create FIFO\n");
    else 
        printf("FIFO '%s' sucessfully created\n",publicFifoName);

    
    if ((fd=open(publicFifoName,O_RDONLY | O_NONBLOCK)) != -1)
        printf("FIFO '%s' opened in READONLY mode\n",publicFifoName);
    else{
        printf("Can't open FIFO\n");
        if (unlink(publicFifoName)<0) printf("Error when destroying FIFO '%s'\n",publicFifoName);
        else printf("FIFO '%s' has been destroyed\n",publicFifoName);
    }
    
    cm.bathroom_time=args.nsecs;

    while(elapsed_time()<args.nsecs){
        if(read(fd,&msg,MAX_NAME_LEN) > 0 && msg[0] == '['){
            strcpy(cm.msg,msg);
            pthread_t t;
            pthread_create(&t,NULL,func,(void *)&cm);
            pthread_detach(t);
        }
        
    }
    pthread_mutex_lock(&mutex2);
    closed=1;
    pthread_mutex_unlock(&mutex2);
    close(fd);
    
    if (unlink(publicFifoName)<0)
        printf("Error when destroying FIFO '%s'\n",publicFifoName);

    printf("Destroyed FIFO '%s'\n",publicFifoName);
    printf("CLOSED BATHROOM! Time : %f\n", elapsed_time());
    exit(0);
} 