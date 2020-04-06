#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>

#include "args.h"
#include "register.h"

#define COMMAND_SIZE 200
#define MAX_NUM_COMMANDS 10
#define ARGS (char*[13]){"-a", "--all", "-b", "--bytes", "-B", "--block-size", "-l", "--count-links", "-L", "--deference", "-S", "--separate-dirs", "--max-depth"}
#define ARGS_SIZE 13
#define READ 0
#define WRITE 1

// Argument struct
struct Args args = {0, 0, 1024, 0, 0, 0, -1};

long subFolderSize;

// Check if a path is valid (return -1), a directory (return 1) or a file (return 0)
int check_path(const char *path) {
    struct stat path_stat;
    if (lstat(path, &path_stat) < 0) // Invalid path
        return -1;
    else if (S_ISDIR(path_stat.st_mode)) // Directory
        return 1;
    else if (S_ISREG(path_stat.st_mode)) // File
        return 0;
    else if(S_ISLNK(path_stat.st_mode))
        return 2;
    else
        return -1;
}

void print_path(char * path, long pathSize) {
    if (args.bytes)
        printf("%ld\t", pathSize);
    else
        printf("%ld\t",pathSize % args.block_size == 0 ? pathSize / args.block_size : pathSize / args.block_size + 1 );
    
    printf("%s\n",path);
        
}

long file_size(char * filename) {
    struct stat fileStat;
    char buf[1024];
    ssize_t len;
    if (stat(filename, &fileStat) < 0) {
        printf("Error reading file stat.\n");
        logExit(1);
    }
    if((check_path(filename)==2) && args.deference){
        lstat(filename,&fileStat);
        if((len=readlink(filename,buf,sizeof(buf)-1))!=-1){
            buf[len]='\0';
            strcat(filename,"/");
            strcat(filename,buf);
        }
    }

    return fileStat.st_size;
}

long print_directory(char * path) {
    DIR * midir;
    struct dirent * info_archivo;
    char fullpath[256], temp[256];
    long dirSize = 0;
    long size = 0;
    
    if ((midir=opendir(path)) == NULL) {
        perror("Error opening directory");
        logExit(1);
    }
    
    while ((info_archivo = readdir(midir)) != 0) {
        strcpy (fullpath, path);
        strcat (fullpath, "/");
        strcat (fullpath, info_archivo->d_name);
        
        if (strcmp(info_archivo->d_name,"..")) {
            strcpy(temp, fullpath);
            size = file_size(fullpath);
            if(check_path(temp)==2 || check_path(temp)==0) {
                if (args.all) print_path(temp, size);
                dirSize += size;
            }
            else if (!strcmp(info_archivo->d_name,"."))
                dirSize += size;
        }
    }

    closedir(midir);
    return dirSize;
}

int folders_in_directory(char * path, char directories[1024][256]) {
    DIR * midir;
    struct dirent * info_archivo;
    char fullpath[256];
    
    int i=0;
    if ((midir=opendir(path)) == NULL) {
        perror("Error opening directory");
        logExit(1);
    }
    
    while ((info_archivo = readdir(midir)) != 0) {
        strcpy (fullpath, path);
        strcat (fullpath, "/");
        strcat (fullpath, info_archivo->d_name);
        if(check_path(fullpath)==1 && strcmp(info_archivo->d_name,".") && strcmp(info_archivo->d_name,"..")){
            strcpy(directories[i],fullpath);
            i++;
        }
    }

    closedir(midir);
    return i;
}

// Searches in a directory and returns size in bytes
int search_dir(char * path, int first) {
    long folderSize = 0, subFolderSize;
    char directories[1024][256];
    int status;
    int num_dir = folders_in_directory(path, directories);

    int my_pipe[2];
    pipe(my_pipe);

    for(int i=0;i<num_dir;i++){
        pid_t pid;
        pid=fork(); 

        if(pid<0){
            printf("Fork failed\n");
            logExit(-1);
        }
        else if (pid == 0){
            folderSize += search_dir(directories[i], 0);
            write(my_pipe[WRITE], &folderSize, sizeof(long));
            close(my_pipe[WRITE]);
            logExit(0);
        }
        else{
            if (first) {close(my_pipe[WRITE]);}
            wait(&status);
            if (read(my_pipe[READ], &subFolderSize, sizeof(long)))
                folderSize += subFolderSize;
            if (i == num_dir - 1) {
                close(my_pipe[READ]);
            }
        }

    }

    folderSize += print_directory(path);

    print_path(path, folderSize);

    return folderSize;
}

// Check if 'element' is in 'arr' of size 'arr_size' (return indice)
int check_in_array(char *arr[], int arr_size, char *element) {
    for(int i = 0; i < arr_size; i++) {
        if (!strcmp(arr[i], element))
            return i;
    }
    return -1;
}

// Activate the flag in args struct (return 0 if success, -1 otherwise)
int activate_flag(char *arg, int num) {
    if (!strcmp(ARGS[0], arg) || !strcmp(ARGS[1], arg))
        if (args.all == 1) return -1;
        else args.all = 1;
    else if (!strcmp(ARGS[2], arg) || !strcmp(ARGS[3], arg))
        if (args.bytes == 1) return -1;
        else args.bytes = 1;
    else if (!strcmp(ARGS[4], arg) || !strcmp(ARGS[5], arg))
        if (args.block_size != 1024) return -1;
        else args.block_size = num;
    else if (!strcmp(ARGS[6], arg) || !strcmp(ARGS[7], arg))
        if (args.countLinks == 1) return -1;
        else args.countLinks = 1;
    else if (!strcmp(ARGS[8], arg) || !strcmp(ARGS[9], arg))
        if (args.deference == 1) return -1;
        else args.deference = 1;
    else if (!strcmp(ARGS[10], arg) || !strcmp(ARGS[11], arg))
        if (args.separateDirs == 1) return -1;
        else args.separateDirs = 1;
    else if (!strcmp(ARGS[12], arg))
        if (args.max_depth != -1) return -1; 
        else args.max_depth = num;
    else
        return -1;

    return 0;
}

int isNumber(char *number)
{
    int i = 0;
    for (; number[i] != 0; i++)
    {
        //if (number[i] > '9' || number[i] < '0')
        if (!isdigit(number[i]))
            return 0;
    }
    return 1;
}

// Check if the arguments are valid
int check_args(int argc, char *argv[]) {
    int path_found = 0;
    int blck_sz=0;          //Used to find the block size, but also has a default number for 'activate_flag' function
    char *elmnt_cpy;

    for (int i = 1; i < argc; i++) {
        elmnt_cpy=strtok(argv[i],"=");
        int pos = check_in_array(ARGS, ARGS_SIZE, elmnt_cpy);
        if (pos >= 0) { // Check valid Argument
            if(pos==5 || pos==12){                  //If the arg is '--block-size' or '--max-depth'
                blck_sz= atoi(strtok(NULL,"="));
            }
            else if(pos==4){                           //If the arg is '-B'
                blck_sz=atoi(argv[i+1]);
                i++;
            }
            
            if (activate_flag(elmnt_cpy, blck_sz) < 0) {
                printf("O argumento que falhou (activate flag): %s\n", elmnt_cpy);
                return 0;
            }
            continue;
        }
        else {
            if (!path_found && check_path(argv[i]) >= 0) {
                path_found = 1;
                strncpy(args.path, argv[i], sizeof(args.path));
            }
            else {
                printf("O argumento que falhou: %s\n", argv[i]);
                return 0;
            }
        }
        blck_sz=0;
    }

    if (!path_found) {
        printf("Missing path\n");
        return 0;
    }

    return 1;
}

int main(int argc, char *argv[], char *envp[])
{
    initLogs();

    if (argc > MAX_NUM_COMMANDS || !check_args(argc, argv)) {
        fprintf(stderr, "Usage: %s -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n", argv[0]);
        exit(1);
    }

    logCreate(argc, argv); 

    //printf("ARGS = {%d, %d, %d, %d, %d, %d, %d}\n", args.all, args.bytes, args.block_size, args.countLinks, args.deference, args.separateDirs, args.max_depth);
    
    search_dir(args.path, 1);

    logExit(0);
}