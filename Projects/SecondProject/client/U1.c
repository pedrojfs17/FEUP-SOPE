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

#define MAX_MSG_LEN 255
#define NUM_THREADS_MAX 100

int i=0;

void *threader(void * arg){
    printf("Entered THREAD\n");
    char *fifo=arg;
    int fd=open(fifo,O_WRONLY);
    printf("Opened Fifo: %s\n",fifo);
    
    if(fd==-1){
        printf("Oops !!! Service is closed !!!\n");
        return NULL;
    }
    printf("FIFO '%s' openned in WRITEONLY mode\n",fifo);

    char msg[MAX_MSG_LEN];
    time_t t;
    srand((unsigned)time(&t));

    sprintf(msg,"[ %d, %d, %ld, %d, -1]",i,getpid(),pthread_self(),rand()%20+1);
    printf("Message to send: %s\n",msg);
    write(fd,&msg,MAX_MSG_LEN);
    close(fd);

    char* private_fifo="/tmp";
    char buff[MAX_MSG_LEN];
    sprintf(buff,"%d",getpid());
    strcat(private_fifo,buff);
    strcat(private_fifo,".");
    sprintf(buff,"%ld",pthread_self());
    strcat(private_fifo,buff);

    int fd_dummy;
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

    if (unlink(private_fifo)<0)
        printf("Error when destroying FIFO '%s'\n",private_fifo);
    else
        printf("FIFO '%s' has been destroyed\n",private_fifo);

    return NULL;
}

int main(int argc, char *argv[]){
    
    pthread_t threads[NUM_THREADS_MAX];
    int t=0;


    if (argc!=4) {
        printf("Usage: U1 <-t secs> fifoname\n");
        exit(1);
    }
    printf("Time of execution: %s\tFifoname:%s\n",argv[2],argv[3]);

    struct timeval begin,end;
    gettimeofday(&begin,NULL); gettimeofday(&end,NULL);
    double nsecs;
    sscanf(argv[2],"%lf",&nsecs);
    double time_taken=(end.tv_sec-begin.tv_sec)*1e6;
    time_taken=(time_taken+(end.tv_usec-begin.tv_usec))*1e6;

    char fifo_copy[MAX_MSG_LEN]="../server/";
    strcat(fifo_copy,argv[3]);
    printf("This is fifo: %s\n",fifo_copy);
    printf("Commencing cycle\n");
    while(time_taken<nsecs){
        pthread_create(&threads[t],NULL,threader,&fifo_copy);
        pthread_join(threads[t],NULL);
        usleep(50000);
        gettimeofday(&end,NULL);
        time_taken=(end.tv_sec-begin.tv_sec)*1e6;
        time_taken=(time_taken+(end.tv_usec-begin.tv_usec))*1e-6;
        printf("Time ocurred: %f\n",time_taken);

    }
    printf("FInished work\n");

    pthread_exit(0);
} 