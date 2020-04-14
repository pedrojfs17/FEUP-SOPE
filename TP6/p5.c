// PROGRAMA p05.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void * sum(void * arg)
{
    int * ptr = (int *)arg;

    /*printf("%d + %d = %d\n",ptr[0],ptr[1],ptr[0]+ptr[1]);
    return NULL;*/

    void * ret = malloc(sizeof(int));
    *(int *)ret = ptr[0]+ptr[1]; return ret; 
}

void * sub(void * arg)
{
    int * ptr = (int *)arg;

    /*printf("%d - %d = %d\n",ptr[0],ptr[1],ptr[0]-ptr[1]);
    return NULL;*/

    void * ret = malloc(sizeof(int));
    *(int *)ret = ptr[0]-ptr[1]; return ret; 
}

void * prod(void * arg)
{
    int * ptr = (int *)arg;

    /*printf("%d * %d = %d\n",ptr[0],ptr[1],ptr[0]*ptr[1]);
    return NULL;*/

    void * ret = malloc(sizeof(int));
    *(int *)ret = ptr[0]*ptr[1]; return ret; 
}

void * quo(void * arg)
{
    int * ptr = (int *)arg;

    /*printf("%d / %d = %d\n",ptr[0],ptr[1],ptr[0]/ptr[1]);
    return NULL;*/

    void * ret = malloc(sizeof(int));
    *(int *)ret = ptr[0]/ptr[1]; return ret; 
}

int main()
{
    int numbers[2];
    printf("x y ? "); scanf("%d %d",&numbers[0],&numbers[1]);

    pthread_t tsum, tsub, tprod, tquo;

    pthread_create(&tsum, NULL, sum, &numbers);
    pthread_create(&tsub, NULL, sub, &numbers);
    pthread_create(&tprod, NULL, prod, &numbers);
    pthread_create(&tquo, NULL, quo, &numbers);

    void * sum, * sub, * prod, * quo;

    pthread_join(tsum, &sum);
    pthread_join(tsub, &sub);
    pthread_join(tprod, &prod);
    pthread_join(tquo, &quo);

    printf("%d + %d = %d\n",numbers[0],numbers[1],*(int*)sum);
    printf("%d - %d = %d\n",numbers[0],numbers[1],*(int*)sub);
    printf("%d * %d = %d\n",numbers[0],numbers[1],*(int*)prod);
    printf("%d / %d = %d\n",numbers[0],numbers[1],*(int*)quo);

    return 0;
} 