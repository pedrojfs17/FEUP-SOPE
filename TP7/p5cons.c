#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h> // For O_* constants
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/mman.h>
#include <sys/types.h>

#define SHM_NAME "/shm1"
#define BUFSIZE 5
#define NUMITEMS 50

typedef struct {
    pthread_mutex_t buffer_lock;
    pthread_cond_t slots_cond;
    pthread_cond_t items_cond;
    pthread_mutex_t slots_lock;
    pthread_mutex_t items_lock;
    int buffer[BUFSIZE];
    int bufin;
    int bufout;
    int items;
    int slots;
    int sum;
} Shared_memory;

Shared_memory * attach_shared_memory(char* shm_name, int shm_size) {
    int shmfd;
    Shared_memory *shm;

    shmfd = shm_open(SHM_NAME,O_RDWR,0660);
    if(shmfd<0) {
        perror("Failure in shm_open()");
        return NULL;
    }

    //attach this region to virtual memory
    shm = mmap(0,shm_size,PROT_READ|PROT_WRITE,MAP_SHARED,shmfd,0);
    if(shm == MAP_FAILED) {
        perror("Failure in mmap()"); 
        return NULL;
    }

    return (Shared_memory *) shm;
} 

void get_item(int *itemp, Shared_memory *shm) {
    pthread_mutex_lock(&shm->buffer_lock);
    *itemp = shm->buffer[shm->bufout];
    shm->bufout = (shm->bufout + 1) % BUFSIZE;
    pthread_mutex_unlock(&shm->buffer_lock);
    return;
} 

void *consumer(void *arg)
{
    int myitem;
    int i;
    Shared_memory *shm = arg;
    printf("In consumer thread\n");

    for (i = 1; i <= NUMITEMS; i++) {
        // wait for an item to be available
        pthread_mutex_lock(&shm->items_lock);
        printf("Consumer: available items = %d\n",shm->items);

        while(!(shm->items > 0))
            pthread_cond_wait(&shm->items_cond, &shm->items_lock);

        shm->items--;
        pthread_mutex_unlock(&shm->items_lock);

        // consume an item
        get_item(&myitem,shm);
        printf("Consumer: consumed item %3d\n",myitem);
        shm->sum += myitem;

        //update num. available slots and notify producer
        pthread_mutex_lock(&shm->slots_lock);
        shm->slots++;
        pthread_cond_signal(&shm->slots_cond);
        pthread_mutex_unlock(&shm->slots_lock);
    }

    pthread_exit(NULL);
} 

int main(void) {
    pthread_t constid;
    Shared_memory *shmem;
    printf("\nCONSUMER: starting after 10 seconds ...\n");
    sleep(10); 

    if ((shmem = attach_shared_memory(SHM_NAME, sizeof(Shared_memory))) == NULL) {
        perror("CONSUMER: could not attach shared memory");
        exit(EXIT_FAILURE);
    }
    
    if (pthread_create(&constid, NULL, consumer, shmem)) {
        perror("CONSUMER: could not create consumer");
        exit(EXIT_FAILURE);
    }

    pthread_join(constid, NULL);
    printf("CONSUMER: the threads produced the sum %d\n", shmem->sum);

    if (munmap(shmem,sizeof(Shared_memory)) < 0) {
        perror("Failure in munmap()");
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
} 