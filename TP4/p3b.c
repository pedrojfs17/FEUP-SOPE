// PROGRAMA p3.c 
#include <stdio.h> 
#include <time.h>
#include <signal.h> 
#include <unistd.h> 
#include <stdlib.h> 

int v;

void sigusr_handler(int signo) { 
    if(signo==SIGUSR1)
        v++;
    if(signo==SIGUSR2)
        v--;
} 

int main(void) { 
    srand(time(NULL)); 
    v = 0; 
    int count = 0;

    struct sigaction action;
    action.sa_handler=sigusr_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags=0;

    if (sigaction(SIGUSR1,&action,NULL) < 0 ||sigaction(SIGUSR2,&action,NULL) < 0)   {     
        fprintf(stderr,"Unable to install SIGUSR handler\n");     
        exit(1);   
    }

    int pid = fork();
    int num;
    while(count < 50){
        if (pid > 0) {
            num = rand() % 2;
            if (num == 0)
                kill(pid, SIGUSR1);
            else
                kill(pid, SIGUSR2);
            sleep(1);
        }
        else {
            pause();
            printf("Value = %d ; Count = %d\n",v, count);
            count++;
        }
    }

    if (pid == 0)
        exit(0);

    exit(0); 
} 