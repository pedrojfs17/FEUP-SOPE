// PROGRAMA p6.c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    int counter = 0;

    while (counter < 30) {
        printf("%s\n",argv[1]);
        counter += 5;
        sleep(5);
    }

    return 0;
}

