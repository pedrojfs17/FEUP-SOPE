#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "signals.h"
#include "args.h"

#define COMMAND_SIZE 200
#define MAX_NUM_COMMANDS 10
#define ARGS (char*[13]){"-a", "--all", "-b", "--bytes", "-B", "--block-size", "-l", "--count-links", "-L", "--deference", "-S", "--separate-dirs", "--max-depth"}
#define ARGS_SIZE 13
#define READ 0
#define WRITE 1

// Argument struct
struct Args args;

int check_file(char * path);
int check_path(struct stat * path_stat);
long calculate_file_size(struct stat * file_stat);
long verifyFolderPath(struct dirent * info, char * path, int * check);
void checkDereference(char *fullpath, struct stat * file_stat);
int check_in_array(char *arr[], int arr_size, char *element);
int activate_flag(char *arg, int num);
int isNumber(char *number);
int check_args(int argc, char *argv[]);