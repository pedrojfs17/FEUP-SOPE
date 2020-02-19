#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[])
{
    int limite = atoi(argv[1]);
    int num = atoi(argv[2]);
    int current, iteration = 1;

    srand(time(NULL));
        
    while (current != num) {
        current = rand() % limite;
        printf("Iteration %d: %d\n", iteration, current);
        iteration++;
    }

    return 0;
} 