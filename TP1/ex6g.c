#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#define BUF_LENGTH 256

int main(int argc, char* argv[])
{
    FILE *src, *dst;
    char buf[BUF_LENGTH];

    if ( ( src = fopen( argv[1], "r" ) ) == NULL ) {
        printf("Error number: %d\n", errno);
        exit(1);
    }

    if ( ( dst = fopen( argv[2], "w" ) ) == NULL ) {
        printf("Error number: %d\n", errno);
        exit(2);
    }

    while( ( fgets( buf, BUF_LENGTH, src ) ) != NULL ) {
        fputs( buf, dst );
    }

    fclose( src );
    fclose( dst );
    exit(0); // zero é geralmente indicativo de "sucesso"
} 