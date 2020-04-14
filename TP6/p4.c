// PROGRAMA p03.c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 10

void *printHello(void *threadId)
{
    int value = *(int*)threadId;
    sleep(1);
    printf("Thread %2d: Hello World!\n", value);

    void * ret = malloc(sizeof(int));
    *(int *)ret = value; return ret; 
}

int main()
{
    pthread_t tid[NUM_THREADS];
    int t; 
    int thrArg[NUM_THREADS]; 
    for(t = 1; t <= NUM_THREADS; t++){
        printf("Creating thread %d\n", t);
        thrArg[t-1] = t; 
        pthread_create(&tid[t-1], NULL, printHello, &thrArg[t-1]);
    }

    for(t = 1; t <= NUM_THREADS; t++){
        void * ret;
        pthread_join(tid[t-1], &ret);
        thrArg[t-1] = *(int*)ret;
        printf("Received %d from return.\n",thrArg[t-1]);
    }

    pthread_exit(NULL);
}