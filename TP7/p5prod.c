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

Shared_memory * create_shared_memory(char* shm_name, int shm_size) {
    int shmfd;
    Shared_memory *shm;

    //create the shared memory region
    shmfd = shm_open(SHM_NAME,O_CREAT|O_RDWR,0660); // try with O_EXCL 
    if(shmfd<0) {
        perror("Failure in shm_open()");
        return NULL;
    }

    //specify the size of the shared memory region
    if (ftruncate(shmfd,shm_size) < 0) {
        perror("Failure in ftruncate()");
        return NULL;
    } 

    //attach this region to virtual memory
    shm = mmap(0,shm_size,PROT_READ|PROT_WRITE,MAP_SHARED,shmfd,0);

    if(shm == MAP_FAILED) {
        perror("Failure in mmap()");
        return NULL;
    }

    //initialize data in shared memory
    shm->bufin = 0;
    shm->bufout = 0;
    shm->items = 0;
    shm->slots = BUFSIZE;
    shm->sum = 0;
    return (Shared_memory *) shm;
}

void destroy_shared_memory(Shared_memory *shm, int shm_size) {
    if (munmap(shm,shm_size) < 0) {
        perror("Failure in munmap()");
        exit(EXIT_FAILURE);
    }
    if (shm_unlink(SHM_NAME) < 0) {
        perror("Failure in shm_unlink()");
        exit(EXIT_FAILURE);
    }
}

void init_sync_objects_in_shared_memory(Shared_memory *shm) {
    pthread_mutexattr_t mattr;
    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shm->buffer_lock, &mattr);
    pthread_mutex_init(&shm->slots_lock, &mattr);
    pthread_mutex_init(&shm->items_lock, &mattr);
    pthread_condattr_t cattr;
    pthread_condattr_init(&cattr);
    pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&shm->slots_cond, &cattr);
    pthread_cond_init(&shm->items_cond, &cattr);
}

void put_item(int item, Shared_memory *shm) {
    pthread_mutex_lock(&shm->buffer_lock);
    shm->buffer[shm->bufin] = item;
    shm->bufin = (shm->bufin + 1) % BUFSIZE;
    pthread_mutex_unlock(&shm->buffer_lock);
    return;
} 

void *producer(void * arg) {
    int i;
    Shared_memory *shm = arg;
    printf("In producer thread\n");

    for (i = 1; i <= NUMITEMS; i++) {
        // wait for a slot to be available
        pthread_mutex_lock(&shm->slots_lock);
        printf("Producer: available slots = %d\n",shm->slots);

        while (!(shm->slots > 0))
            pthread_cond_wait (&shm->slots_cond, &shm->slots_lock);

        shm->slots--;
        pthread_mutex_unlock(&shm->slots_lock);
        // produce item
        put_item(i,shm);
        printf("Producer: produced item %3d\n",i);

        // update num. produced items and notify consumer
        pthread_mutex_lock(&shm->items_lock);
        shm->items++;
        pthread_cond_signal(&shm->items_cond);
        pthread_mutex_unlock(&shm->items_lock);
    }

    pthread_exit(NULL);
}

int main(void){
    pthread_t prodtid;
    int i, total;
    Shared_memory *shmem;

    printf("\nPRODUCER: starting after 5 seconds ...\n");
    sleep(5);
    if ((shmem = create_shared_memory(SHM_NAME, sizeof(Shared_memory))) == NULL) {
        perror("PRODUCER: could not create shared memory");
        exit(EXIT_FAILURE);
    }

    init_sync_objects_in_shared_memory(shmem);
    // NOT NECESSARILY A THREAD ... COULD BE JUST A CALL TO producer() FUNCTION
    if (pthread_create(&prodtid, NULL, producer, shmem)) {
        exit(EXIT_FAILURE);
    }

    pthread_join(prodtid, NULL);
    destroy_shared_memory(shmem, sizeof(Shared_memory)); 

    total = 0;
    
    for (i = 1; i <=NUMITEMS; i++)
        total += i;
    
    printf("PRODUCER: the checksum is %d\n", total);
    exit(EXIT_SUCCESS);
} 