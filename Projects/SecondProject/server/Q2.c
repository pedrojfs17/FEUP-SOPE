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
#include <semaphore.h>
#include "../utils/defs.h"
#include "../utils/logs.h"
#include "../utils/utils.h"
#include "../utils/queue.h"

int place=1;
int closed=0;

int activateMaxThreads = 0;
int activateMaxPlaces = 0;

sem_t nMaxThreads;
sem_t nMaxPlaces;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

queue q;

void * func(void * arg){
    pthread_detach(pthread_self());

    int privateFifo;
    struct cln_msg *cop;
    cop=(struct cln_msg *) arg;

    char privateFifoName[MAX_NAME_LEN];
    int i, duration, pid; long tid;
    sscanf(cop->msg,"[ %d, %d, %ld, %d, -1]",&i,&pid,&tid,&duration);
    writeRegister(i, pid, tid, duration, -1, RECEIVED);

    sprintf(privateFifoName, "/tmp/%d.%ld", pid, tid);
    
    if ((privateFifo=open(privateFifoName,O_WRONLY|O_NONBLOCK)) < 0) {
        writeRegister(i, pid, tid, duration, -1, GAVEUP);
        if (activateMaxThreads) sem_post(&nMaxThreads);
        pthread_exit(NULL);
    }

    int my_place;
    if(activateMaxPlaces){
        sem_wait(&nMaxPlaces);
        pthread_mutex_lock(&mutex);
        my_place = occupy(&q);
        pthread_mutex_unlock(&mutex);
    }
    else{
        pthread_mutex_lock(&mutex);
        my_place = place;
        place++;
        pthread_mutex_unlock(&mutex);
    }
    

    char client_msg[MAX_NAME_LEN];

    if(elapsed_time()<cop->bathroom_time){
        sprintf(client_msg,"[ %d, %d, %ld, %d, %d]",i,getpid(),pthread_self(),duration,my_place);
        writeRegister(i, getpid(), pthread_self(), duration, my_place, ENTER);
    }
    else {
        sprintf(client_msg,"[ %d, %d, %ld, %d, %d]",i,getpid(),pthread_self(),-1,-1);
        writeRegister(i, getpid(), pthread_self(), duration, -1, TOOLATE);
    }

    if(write(privateFifo,&client_msg,MAX_NAME_LEN)<0){
        fprintf(stderr, "Error writing to private fifo of request %d\n",i);
        writeRegister(i, pid, tid, duration, -1, GAVEUP);
        if(close(privateFifo)<0) fprintf(stderr, "Error closing private fifo of request %d\n",i);
        if (activateMaxThreads) sem_post(&nMaxThreads);
        if (activateMaxPlaces) {
            pthread_mutex_lock(&mutex);
            deoccupy(&q,my_place);
            pthread_mutex_unlock(&mutex);
            sem_post(&nMaxPlaces);
        }
        pthread_exit(NULL);
    }
    if(close(privateFifo)<0){
        fprintf(stderr, "Error closing private fifo of request %d\n",i);
        pthread_exit(NULL);
    }

    if(!closed){
        usleep(duration*1000);
        //fprintf(stderr, "Waiting\n");
        writeRegister(i, getpid(), pthread_self(), duration, my_place, TIMEUP);
    }
    
    if (activateMaxThreads) sem_post(&nMaxThreads);

    if (activateMaxPlaces) {
        pthread_mutex_lock(&mutex);
        deoccupy(&q,my_place);
        pthread_mutex_unlock(&mutex);
        sem_post(&nMaxPlaces);
    }

    pthread_exit(NULL);
}

int main(int argc, char*argv[]){
    int fd;
    char msg[MAX_NAME_LEN];
    char publicFifoName[MAX_NAME_LEN]="server/";
    struct cln_msg cm;
    srv_args args;
    args.nplaces = 0;
    args.nsecs = 0;
    args.nthreads = 0;
    
    if(check_server_arg(&args,argc,argv)==-1){
        fprintf(stderr, "Usage: Q1 <-t secs> [-l nplaces] [-n nthreads] fifoname\n");
        exit(1);
    }

    if (args.nplaces > 0) activateMaxPlaces = 1;
    if (args.nthreads > 0) activateMaxThreads = 1;

    initClock();
    
    strcat(publicFifoName,args.fifoname);
    fprintf(stderr, "Time of execution: %d\tFifoname:%s\n",args.nsecs,publicFifoName);

    // Create public FIFO
    if (mkfifo(publicFifoName,0660)<0)
        if (errno == EEXIST) fprintf(stderr, "FIFO '%s' already exists\n",publicFifoName);
        else fprintf(stderr, "Can't create FIFO\n");
    else 
        fprintf(stderr, "FIFO '%s' sucessfully created\n",publicFifoName);

    
    // Open public FIFO
    if ((fd=open(publicFifoName,O_RDONLY | O_NONBLOCK)) != -1)
        fprintf(stderr, "FIFO '%s' opened in READONLY mode\n",publicFifoName);
    else{
        fprintf(stderr, "Can't open FIFO\n");
        if (unlink(publicFifoName)<0) fprintf(stderr, "Error when destroying FIFO '%s'\n",publicFifoName);
        else fprintf(stderr, "FIFO '%s' has been destroyed\n",publicFifoName);
    }
    
    cm.bathroom_time=args.nsecs;

    if (activateMaxThreads) {
        sem_init(&nMaxThreads, 0, args.nthreads);
    }
    if (activateMaxPlaces) {
        sem_init(&nMaxPlaces, 0, args.nplaces);
        q = createQueue(args.nplaces);
        fillQueue(&q);
    }

    while (elapsed_time() < args.nsecs){
        if (read(fd,&msg,MAX_NAME_LEN) > 0 && msg[0] == '['){
            if (activateMaxThreads) sem_wait(&nMaxThreads);
            strcpy(cm.msg,msg);
            pthread_t t;
            pthread_create(&t,NULL,func,(void *)&cm);
        }
    }
    
    closed=1;
    if(close(fd)<0)
        fprintf(stderr, "Error when closing public FIFO\n");
    
    if (unlink(publicFifoName)<0)
        fprintf(stderr, "Error when destroying FIFO '%s'\n",publicFifoName);

    while (read(fd, &msg, MAX_NAME_LEN) > 0) {
        if (msg[0] == '[') {
            if (activateMaxThreads) sem_wait(&nMaxThreads);
            strcpy(cm.msg,msg);
            pthread_t t;
            pthread_create(&t,NULL,func,(void *)&cm);
        }
    }

    fprintf(stderr, "Destroyed FIFO '%s'\n",publicFifoName);
    fprintf(stderr, "CLOSED BATHROOM! Time : %f\n", elapsed_time());
    pthread_exit(0);
} 