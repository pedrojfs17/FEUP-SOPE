// PROGRAMA p01.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/types.h>

#define MAXELEMS 10000000 // nr. max de posicoes
#define MAXTHREADS 100 // nr. max de threads
#define min(a, b) (a)<(b)?(a):(b)

#define SHM_SIZE 10

char SEM_NAME[] = "/sem1";
char SHM_NAME[] = "/shm1";

int npos;
int buf[MAXELEMS], pos=0, val=0; // variaveis partilhadas

void *fill(void *nr) {
    while (1) {
        pthread_mutex_lock(&mut);
        if (pos >= npos) {
            pthread_mutex_unlock(&mut);
            return NULL;
        }
        buf[pos] = val;
        pos++; val++;
        pthread_mutex_unlock(&mut);
        *(int *)nr += 1;
    }
}

void *verify(void *arg) {
    int k;
    for (k=0; k<npos; k++)
        if (buf[k] != k) // detecta valores errados
            printf("ERROR: buf[%d] = %d\n", k, buf[k]);
    return NULL;
}

int main(int argc, char *argv[]) {
    int shmfd;
    char * shm, *s;
    sem_t *sem;

    shmfd = shm_open(SHM_NAME, O_CREAT|O_RDWR, 0600);
    if (shmfd < 0) {
        perror("WRITER failure in shm_open()");
        exit(1);
    }
    if (ftruncate(shmfd, SHM_SIZE) < 0) {
        perror("WRITER failure in ftruncate()");
        exit(2);
    }
    shm = (char *) mmap(0,SHM_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,shmfd,0);
    if(shm == MAP_FAILED) {
        perror("WRITER failure in mmap()");
        exit(3);
    }

    int k, nthr, count[MAXTHREADS]; // array para contagens
    int total;

    if (argc != 3) {
        printf("Usage: %s <nr_pos> <nr_thrs>\n",argv[0]);
        return 1;
    } 

    npos = min(atoi(argv[1]), MAXELEMS); //no. efectivo de posicoes
    nthr = min(atoi(argv[2]), MAXTHREADS); //no. efectivo de threads
    
    for (k=0; k<nthr; k++) { // criacao das threads 'fill'
        count[k] = 0;
        pthread_create(&tidf[k], NULL, fill, &count[k]);
    }

    total=0;

    for (k=0; k<nthr; k++) { //espera threads 'fill'
        pthread_join(tidf[k], NULL);
        printf("count[%d] = %d\n", k, count[k]);
        total += count[k];
    }

    printf("total count = %d\n",total); // mostra total
    pthread_create(&tidv, NULL, verify, NULL);
    pthread_join(tidv, NULL); // espera thread 'verify'
    
    return 0;
} 