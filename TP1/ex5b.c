#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[], char* envp[])
{
    char * user;
    for (int i = 0; envp[i] != NULL; i++) {
        if (strncmp("USER=", envp[i], 5) == 0) {
            user = strtok(envp[i], "=");
            user = strtok(NULL, "=");
            printf("Hello %s!\n", user);
        }
    }
    return 0;
}