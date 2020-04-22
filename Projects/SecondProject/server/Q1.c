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

int place=1;
int occupied=0;
int time_wait=0;


void * func(void * arg){
    int fd_dummy;
    struct cln_msg *cop;
    cop=(struct cln_msg *) arg;

    printf("Message Received: %s\n",cop->msg);

    char *fifo_name="/tmp",pid[MAX_NAME_LEN],tid[MAX_NAME_LEN];
    int i,duration;
    sscanf(cop->msg,"[ %d, %s, %s, %d, -1]",&i,pid,tid,&duration);

    strcat(fifo_name,pid);
    strcat(fifo_name,".");
    strcat(fifo_name,tid);
    
    
    fd_dummy=open(fifo_name,O_WRONLY);
    char client_msg[MAX_NAME_LEN];
    sprintf(client_msg,"[ %d, %d, %ld, %d, %d]",i,getpid(),pthread_self(),duration,place++);
    printf("Sending message: %s",client_msg);
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
    
    strcpy(fifo_cpy,argv[3]);

    if (mkfifo(fifo_cpy,0660)<0) //Makes fifoname
        if (errno == EEXIST) printf("FIFO '%s' already exists\n",fifo_cpy);
        else printf("Can't create FIFO\n");
    else 
        printf("FIFO '%s' sucessfully created\n",fifo_cpy);

    
    if ((fd=open(fifo_cpy,O_RDONLY)) !=-1)
        printf("FIFO '%s' opened in READONLY mode\n",fifo_cpy);
    else{
        printf("Can't open FIFO\n");
        if (unlink(fifo_cpy)<0)
        printf("Error when destroying FIFO '%s'\n",fifo_cpy);
        else
            printf("FIFO '%s' has been destroyed\n",fifo_cpy);
    }
    
    struct timeval begin,end;
    gettimeofday(&begin,NULL); gettimeofday(&end,NULL);
    double nsecs;
    sscanf(argv[2],"%lf",&nsecs);
    double time_taken=(end.tv_sec-begin.tv_sec)*1e6;
    time_taken=(time_taken+(end.tv_usec-begin.tv_usec))*1e6;

    do{
        if(read(fd,&msg,MAX_NAME_LEN)){
            strcpy(cm.msg,msg);
            pthread_create(&t,NULL,func,(void *)&cm);
            pthread_join(t,NULL);
        }
        gettimeofday(&end,NULL);
        time_taken=(end.tv_sec-begin.tv_sec)*1e6;
        time_taken=(time_taken+(end.tv_usec-begin.tv_usec))*1e-6;
        //printf("Time ocurred: %f\n",time_taken);
    }while(time_taken<nsecs);

    close(fd);


    if (unlink(argv[3])<0)
        printf("Error when destroying FIFO '%s'\n",argv[3]);
    else
        printf("FIFO '%s' has been destroyed\n",argv[3]);
    
    
    pthread_exit(0);
} 