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

// Check if 'element' is in 'arr' of size 'arr_size'
int check_in_array(char *arr[], int arr_size, char *element) {
    for(int i = 0; i < arr_size; ++i) {
        if (!strcmp(arr[i], element))
            return 1;
    }
    return 0;
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

// Check if the arguments are valid
int check_args(int argc, char *argv[]) {
    int path_found = 0;
    char *temp[14];
    int temp_size = 0;

    for (int i = 1; i < argc; i++) {
        if (check_in_array(ARGS, ARGS_SIZE, argv[i])) { // Check valid Argument
            if (!check_in_array(temp, temp_size, argv[i])) { // Check if it isn't duplicate
                temp[temp_size] = argv[i];
                temp_size++;
            }
            else
                return 0;
        }
        else {
            if (!path_found && check_path(argv[i]) >= 0)
                path_found = 1;
            else
                return 0;
        }
    }

    return 1;
}

int main(int argc, char *argv[], char *envp[])
{
    struct Args args = {0, 0, 1024, 0, 0, 0, -1};

    if (argc > MAX_NUM_COMMANDS || !check_args(argc, argv)) {
        fprintf(stderr, "Usage: %s -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n", argv[0]);
        exit(1);
    }

    return 0;
}