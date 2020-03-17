// PROGRAMA p3.c 
#include <stdio.h> 
#include <signal.h> 
#include <unistd.h> 
#include <stdlib.h> 

int v;

void sigusr_handler(int signo) {   
    printf("In SIGUSRX handler ...\n"); 
    if(signo==SIGUSR1)
        v++;
    if(signo==SIGUSR2)
        v--;
    printf("Exiting SIGUSRX handler ...\n"); 
} 

int main(void) {  
    v = 0; 

    struct sigaction action;
    action.sa_handler=sigusr_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags=0;

    if (sigaction(SIGUSR1,&action,NULL) < 0 ||sigaction(SIGUSR2,&action,NULL) < 0)   {     
        fprintf(stderr,"Unable to install SIGUSR handler\n");     
        exit(1);   
    }

    while(1){
        printf("Value = %d\n",v);
        sleep(1);
    }

    exit(0); 
} 