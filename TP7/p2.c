// PROGRAMA p01.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define STDERR 2

int N = 50000;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER; // mutex p/a sec.critica

void * thrfunc(void * arg)
{
    int i = 0;
    fprintf(stderr, "Starting thread %s\n", (char *) arg);
    while (N > 0) {
        pthread_mutex_lock(&mut);
        N--;
        i++;
        pthread_mutex_unlock(&mut);
        write(STDERR,arg,1);
    }

    void * ret = malloc(sizeof(int));
    *(int *)ret = i;
    return ret;
}

int main()
{
    pthread_t ta, tb;
    void * r1, * r2;

    pthread_create(&ta, NULL, thrfunc, "1");
    pthread_create(&tb, NULL, thrfunc, "2");
    pthread_join(ta, &r1);
    pthread_join(tb, &r2);

    fprintf(stderr, "\nThread 1: %d\n", *(int *) r1);
    fprintf(stderr, "Thread 2: %d\n", *(int *) r2);

    return 0;
} 