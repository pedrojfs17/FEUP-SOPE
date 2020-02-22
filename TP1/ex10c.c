#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() 
{ 
    char str[80]; 
    const char s[2] = " ";
    int i = 0;
    char *token[10];
    char *p;

    fgets(str, 80, stdin); 
    
    p = strtok(str, s);
   
    while (p != NULL) {

        token[i++] = p;
        p = strtok(NULL, s);
    }
  
    for (int j = 0; j < i; j++) {
        if (strchr(token[j], ';') != NULL || strchr(token[j], '|') != NULL) {
            token[j][strlen(token[j])-1] = 0;
            printf(" %s\n", token[j]);
        }
        else
            printf(" %s", token[j]);
    }

    return 0; 
} 
