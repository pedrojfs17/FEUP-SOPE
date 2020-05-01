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
int occupied=0;
int closed=0;
double time_taken = 0.0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void * func(void * arg){
    int fd_dummy;
    struct cln_msg *cop;
    cop=(struct cln_msg *) arg;

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
        //writeRegister(i, pid, tid, duration, -1, GAVEUP);
        //printf("Tryied to open FIFO '%s' and failed. Trying again.\n",fifo_name);
        usleep(1000);
        //return NULL;
    }

    pthread_mutex_lock(&mutex);

    int my_place = place;
    place++;

    pthread_mutex_unlock(&mutex);

    char client_msg[MAX_NAME_LEN];
    if(elapsed_time()+duration*1e-3<cop->bathroom_time){
        sprintf(client_msg,"[ %d, %d, %ld, %d, %d]",i,getpid(),pthread_self(),duration,my_place);
        writeRegister(i, getpid(), pthread_self(), duration, my_place, ENTER);
    }
    else{
        closed=1;
        sprintf(client_msg,"[ %d, %d, %ld, %d, %d]",i,getpid(),pthread_self(),-1,-1);
        writeRegister(i, getpid(), pthread_self(), duration, -1, TOLATE);
    }

    if(write(fd_dummy,&client_msg,MAX_NAME_LEN)<0){
        writeRegister(i, pid, tid, duration, -1, GAVEUP);
        return NULL;
    }
    close(fd_dummy);

    usleep(duration*1000);

    if(!closed)
        writeRegister(i, getpid(), pthread_self(), duration, my_place, TIMEUP);

    return NULL;
}

int main(int argc, char*argv[]){
    int fd;
    char msg[MAX_NAME_LEN];
    char fifo_cpy[MAX_NAME_LEN]="server/";
    struct cln_msg cm;
    srv_args args;
    args.nsecs=0;
    

    if(check_server_arg(&args,argc,argv)==-1){
        printf("Usage: Q1 <-t secs> fifoname\n");
        exit(1);
    }

    initClock();
    
    strcat(fifo_cpy,args.fifoname);
    printf("Time of execution: %d\tFifoname:%s\n",args.nsecs,fifo_cpy);

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
    
    /*double nsecs;
    sscanf(argv[2],"%lf",&nsecs);*/
    cm.bathroom_time=args.nsecs;

    while(elapsed_time()<args.nsecs){
        if(read(fd,&msg,MAX_NAME_LEN) > 0 && msg[0] == '['){
            strcpy(cm.msg,msg);
            pthread_t t;
            pthread_create(&t,NULL,func,(void *)&cm);
            //pthread_join(t,NULL);
            pthread_detach(t);
        }
        
    }
    closed=1;
    close(fd);
    

    if (unlink(fifo_cpy)<0)
        printf("Error when destroying FIFO '%s'\n",fifo_cpy);

    printf("Destroyed FIFO '%s'\n",fifo_cpy);
    printf("CLOSED BATHROOM! Time : %f\n", elapsed_time());
    exit(0);
} 