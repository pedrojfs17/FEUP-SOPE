#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() 
{ 
    char str[80]; 
    const char s[2] = " ";
    char *token;

    fgets(str, 80, stdin); 
    
    token = strtok(str, s);
   
    while (token != NULL) {
        printf(" %s", token);
        token = strtok(NULL, s);
    }
  
    return 0; 
} 
