#include <stdio.h>
#include <stdlib.h>

void function1 () {
    printf("Executing exit handler 1\n");
}

void function2 () {
    printf("Executing exit handler 2\n");
}

void function3 () {
    printf("Executing exit handler 3\n");
    exit(1);
}

int main () {
   /* register the termination function */
    atexit(function1);
    atexit(function2);
    //atexit(function1);
    atexit(function3);
   
    //abort();

    printf("Main done!\n");

    return(0);
}