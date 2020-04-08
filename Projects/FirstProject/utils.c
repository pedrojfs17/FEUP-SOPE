#include "utils.h"

// Argument struct
struct Args args = {0, 0, 1024, 0, 0, 0, __INT_MAX__};


//Checks if the file exists
int check_file(char * path){
    struct stat stat_path;
    if(stat(path,&stat_path)<0){
        return -1;
    }

    if(S_ISDIR(stat_path.st_mode)||S_ISREG(stat_path.st_mode)||S_ISLNK(stat_path.st_mode)){
        return 0;
    }
    else return -1;
}

// Gives type of file
int check_path(struct stat * path_stat) {
    if (S_ISDIR(path_stat->st_mode)) // Directory
        return 1;
    else if (S_ISREG(path_stat->st_mode)) // File
        return 0;
    else if(S_ISLNK(path_stat->st_mode)) //  Link
        return 2;
    else
        return -1;
}

// Calculates the size of a file (in bytes or blocks depending on the flag)
long calculate_file_size(struct stat * file_stat) {
    if (args.bytes) return file_stat->st_size;
    else return file_stat->st_blocks * 512 / args.block_size;
}

// Checks if the path is either '..' or '.'
long verifyFolderPath(struct dirent * info, char * path, int * check){
    struct stat folder_stat;
    if(!strcmp(info->d_name,"..")){(*check)=1; return 0;}
    else if(!strcmp(info->d_name,".")){
        (*check)=1;
        stat(path,&folder_stat);
        return calculate_file_size(&folder_stat);
        
    }
    return 0;
}

// Checks if deference flag is active and if we must do stat or lstat
void checkDereference(char *fullpath, struct stat * file_stat){
    if(args.deference){
        if(stat(fullpath,&(*file_stat))){logExit(1);}
    }
    else{
        if(lstat(fullpath,&(*file_stat))){logExit(1);}
    }
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
        if (args.block_size != 1024 || num < 0) return -1;
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
        if (args.max_depth != __INT_MAX__ || num < 0) return -1; 
        else args.max_depth = num;
    else
        return -1;

    return 0;
}

int isNumber(char *number)
{
    int i = 0;
    for (; number[i] != 0; i++){
        if (!isdigit(number[i]))
            return 0;
    }
    return 1;
}

// Check if the arguments are valid->
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
            if (!path_found && check_file(argv[i]) >= 0) {
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
