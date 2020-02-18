#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[])
{
    printf("Hello %s!\n", getenv("USER"));
    return 0;
}