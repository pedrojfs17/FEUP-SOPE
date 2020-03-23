#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "args.h"

#define COMMAND_SIZE 200
#define MAX_NUM_COMMANDS 10
#define ARGS (const char*[14]){"-a", "--all", "-b", "--bytes", "-B", "--block-size=", "-l", "--count-links", "-L", "--deference", "-S", "--separate-dirs", "--max-depth="}
#define ARGS_SIZE 14

// Argument struct
struct Args args = {0, 0, 1024, 0, 0, 0, -1};


// Check if 'element' is in 'arr' of size 'arr_size' (return indice)
int check_in_array(char *arr[], int arr_size, char *element) {
    for(int i = 0; i < arr_size; ++i) {
        if (!strcmp(arr[i], element))
            return i;
    }
    return -1;
}

// Check if a path is valid (return -1), a directory (return 1) or a file (return 0)
int check_path(const char *path) {
    struct stat path_stat;
    if (stat(path, &path_stat) < 0) // Invalid path
        return -1;
    else if (S_ISDIR(path_stat.st_mode)) // Directory
        return 1;
    else if (S_ISREG(path_stat.st_mode)) // File
        return 0;
    else
        return -1;
}

// Activate the flag in args struct (return 0 if success, -1 otherwise)
int activate_flag(char *arg, int num) {
    if (!strcmp(ARGS[0], arg) || !strcmp(ARGS[1], arg))
        args.all = 1;
    else if (!strcmp(ARGS[2], arg) || !strcmp(ARGS[3], arg))
        args.bytes = 1;
    else if (!strcmp(ARGS[4], arg) || !strcmp(ARGS[5], arg))
        args.block_size = num;
    else if (!strcmp(ARGS[6], arg) || !strcmp(ARGS[7], arg))
        args.countLinks = 1;
    else if (!strcmp(ARGS[8], arg) || !strcmp(ARGS[9], arg))
        args.deference = 1;
    else if (!strcmp(ARGS[10], arg) || !strcmp(ARGS[11], arg))
        args.separateDirs = 1;
    else if (!strcmp(ARGS[12], arg)) 
        args.max_depth = num;
    else
        return -1;

    return 0;
}

/*-----------------------------------------------------------------

TODO

    - Verificar quando é '-B' para enviar o argumento asseguir

TODO BUT MORE DIFICULT (DESEMMERDA-TE)

    - Quando for '--block-size=XXX' ou '--max-depth=XXX' conta como
    um só argumento, ou seja, nao sei como fazer para separar o numero. 

-----------------------------------------------------------------*/


// Check if the arguments are valid
int check_args(int argc, char *argv[]) {
    int path_found = 0;
    char *temp[14];
    int temp_size = 0;

    for (int i = 1; i < argc; i++) {
        int pos = check_in_array(ARGS, ARGS_SIZE, argv[i]);
        if (pos >= 0) { // Check valid Argument
            if (check_in_array(temp, temp_size, argv[i]) < 0) { // Check if it isn't duplicate
                temp[temp_size] = ARGS[pos];
                if (pos == 12)
                    temp_size++;
                else {
                    if (pos % 2 == 0)
                        temp[temp_size + 1] = ARGS[pos + 1];
                    else
                        temp[temp_size + 1] = ARGS[pos - 1];
                    temp_size+=2;
                }
                if (activate_flag(argv[i], 0) < 0) {
                    printf("O argumento que falhou (activate flag): %s\n", argv[i]);
                    return 0;
                }
                continue;
            }
            else {
                printf("O argumento que falhou (check duplicate): %s\n", argv[i]);
                return 0;
            }
        }
        else {
            if (!path_found && check_path(argv[i]) >= 0)
                path_found = 1;
            else {
                printf("O argumento que falhou: %s\n", argv[i]);
                return 0;
            }
        }
    }

    return 1;
}

int main(int argc, char *argv[], char *envp[])
{
    if (argc > MAX_NUM_COMMANDS || !check_args(argc, argv)) {
        fprintf(stderr, "Usage: %s -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n", argv[0]);
        exit(1);
    }

    printf("ARGS = {%d, %d, %d, %d, %d, %d, %d}\n", args.all, args.bytes, args.block_size, args.countLinks, args.deference, args.separateDirs, args.max_depth);

    return 0;
}