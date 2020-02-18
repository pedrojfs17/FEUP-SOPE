#include <stdio.h>

int main(int argc, char* argv[], char* envp[])
{
    printf("variaveis de Ambiente: \n");
    for (int i = 0; envp[i] != NULL; i++) {
        printf("%s\n", envp[i]);
    }
    return 0;
}