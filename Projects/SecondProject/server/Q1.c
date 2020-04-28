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
#include "../utils/logs.h"
#include "../utils/utils.h"

int place=1;
int occupied=0;
int closed=0;
double time_taken = 0.0;


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
        writeRegister(i, pid, tid, duration, -1, GAVEUP);
        printf("Tryied to open FIFO '%s' and failed. Trying again.\n",fifo_name);
        usleep(1000);
    }
    
    char client_msg[MAX_NAME_LEN];
    if(elapsed_time()+duration*1e-3<cop->bathroom_time){
        sprintf(client_msg,"[ %d, %d, %ld, %d, %d]",i,getpid(),pthread_self(),duration,place++);
        writeRegister(i, getpid(), pthread_self(), duration, place - 1, ENTER);
    }
    else{
        closed=1;
        sprintf(client_msg,"[ %d, %d, %ld, %d, %d]",i,getpid(),pthread_self(),-1,-1);
        writeRegister(i, getpid(), pthread_self(), duration, place - 1, TOLATE);
    }

    usleep(duration*1000);
    if(!closed)
        writeRegister(i, getpid(), pthread_self(), duration, place - 1, TIMEUP);
    write(fd_dummy,&client_msg,MAX_NAME_LEN);
    close(fd_dummy);

    return NULL;
}

int main(int argc, char*argv[]){
    int fd;
    char msg[MAX_NAME_LEN];
    char fifo_cpy[MAX_NAME_LEN];
    struct cln_msg cm;
    srv_args args;
    pthread_t t;

    if(check_server_arg(&args,argc,argv)==-1){
        perror("Error in args!\n");
        exit(1);
    }

    initClock();
    
    //strcpy(fifo_cpy,argv[3]);
    printf("Time of execution: %d\tFifoname:%s\n",args.nsecs,args.fifoname);

    if (mkfifo(args.fifoname,0660)<0) //Makes fifoname
        if (errno == EEXIST) printf("FIFO '%s' already exists\n",args.fifoname);
        else printf("Can't create FIFO\n");
    else 
        printf("FIFO '%s' sucessfully created\n",args.fifoname);

    
    if ((fd=open(args.fifoname,O_RDONLY | O_NONBLOCK)) !=-1)
        printf("FIFO '%s' opened in READONLY mode\n",args.fifoname);
    else{
        printf("Can't open FIFO\n");
        if (unlink(args.fifoname)<0)
        printf("Error when destroying FIFO '%s'\n",args.fifoname);
        else
            printf("FIFO '%s' has been destroyed\n",args.fifoname);
    }
    
    /*double nsecs;
    sscanf(argv[2],"%lf",&nsecs);*/
    cm.bathroom_time=args.nsecs;

    do{
        if(read(fd,&msg,MAX_NAME_LEN) > 0 && msg[0] == '['){
            strcpy(cm.msg,msg);
            pthread_create(&t,NULL,func,(void *)&cm);
            //pthread_join(t,NULL);
        }
        
    } while(elapsed_time()<args.nsecs);
    closed=1;
    close(fd);
    

    if (unlink(args.fifoname)<0)
        printf("Error when destroying FIFO '%s'\n",args.fifoname);
    
    printf("CLOSED BATHROOM!\n");
    pthread_exit(0);
} 