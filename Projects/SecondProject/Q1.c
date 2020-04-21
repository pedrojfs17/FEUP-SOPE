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
#include <time.h>
#include "defs.h"

#define MAX_NAME_LEN 50

struct cln_msg{
    char msg[MAX_NAME_LEN];
};

void * func(void * arg){
    int fd_dummy,num1,num2;
    struct cln_msg *cop;
    cop=(struct cln_msg *) arg;
    char fifo_name[MAX_NAME_LEN];
    void *r;
    r=malloc(sizeof(int));
    sscanf(cop->msg,"%d:%d:%s",&num1,&num2,fifo_name);
    *(int *)r=0;

    if(num1!=0 || num2!=0){
        printf("Operands: %d e %d. FIFO name = %s\n",num1,num2,fifo_name);
        fd_dummy=open(fifo_name,O_WRONLY);
        char client_msg[MAX_NAME_LEN];
        sprintf(client_msg,"Sum=%d\tSub=%d\tMult=%d\tDiv=%d\n",num1+num2,num1-num2,num1*num2,num1/num2);
        write(fd_dummy,&client_msg,MAX_NAME_LEN);
        close(fd_dummy);
        *(int *)r=1;
    }
    
    return r;
}

int main(int argc, char*argv[]){
    int fd;
    char msg[MAX_NAME_LEN];
    int num[2];
    struct cln_msg cm;
    pthread_t t;

    void *rt;

    if (argc!=4) {
        printf("Usage: U1 <-t secs> fifoname\n");
        exit(1);
    }
    double secs;
    sscanf(argv[3],"%ld",&secs);
    clock_t begin=clock(),end;

    if (mkfifo(argv[3],0660)<0) //Makes fifoname
        if (errno == EEXIST) printf("FIFO '%s' already exists\n",argv[3]);
        else printf("Can't create FIFO\n");
    else 
        printf("FIFO '%s' sucessfully created\n",argv[3]);
    
    if ((fd=open(argv[3],O_RDONLY)) !=-1)
        printf("FIFO '%s' opened in READONLY mode\n",argv[3]);
    

    do{
        if(read(fd,&msg,MAX_NAME_LEN)){
            strcpy(cm.msg,msg);
            pthread_create(&t,NULL,func,(void *)&cm);
            pthread_join(t,&rt);
        }
        end=clock();
    }while(((double)(end-begin)/CLOCKS_PER_SEC)<secs);
    free(rt);

    close(fd);


    if (unlink(argv[3])<0)
        printf("Error when destroying FIFO '%s'\n",argv[3]);
    else
        printf("FIFO '%s' has been destroyed\n",argv[3]);
    

    
    
    pthread_exit(0);
} 