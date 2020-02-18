#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    int num = atoi(argv[2]);
    while (num > 0) {
        printf("Hello %s!\n", argv[1]);
        num--;
    }
    return 0;
} 