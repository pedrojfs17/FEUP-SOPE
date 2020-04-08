
#include <sys/wait.h>
#include "utils.h"

pid_t ParentPGID;

// Searches in a directory and returns size in bytes
int search_dir(char * path, int depth) {
    long folderSize = 0;
    int status;
    int check_folder_path=0;
    DIR * midir;
    struct dirent * info_archivo;
    char fullpath[256];
    struct stat file_stat;

    //sleep(2);
    
    depth-=1;//Depth flag
    if ((midir=opendir(path)) == NULL) {
        perror("Error opening directory");
        logExit(1);
    }
    
    while ((info_archivo = readdir(midir)) != 0) {
        strcpy (fullpath, path);
        if (fullpath[strlen(fullpath) - 1] != '/') strcat (fullpath, "/");
        strcat (fullpath, info_archivo->d_name);
        check_folder_path = 0;

        folderSize+=verifyFolderPath(info_archivo,fullpath,&check_folder_path); //Adds size of folder

        if(check_folder_path){continue;}
        checkDereference(fullpath,&file_stat);                                  //Checks deference flag

        long subSize;
        if(check_path(&file_stat)==0 || check_path(&file_stat)==2){
            subSize=calculate_file_size(&file_stat);                            //Calculates file/link size
            folderSize+=subSize;
            if(args.all && depth>=0){
                char message[256];
                sprintf(message,"%ld\t%s\n",subSize,fullpath);
                logEntry(message);
                printf("%s\n",message);      //Prints if the depth is not exceeded
                fflush(stdout);
            }
        }
        else if(check_path(&file_stat)==1){                                     //If the file is a directory
            
            int my_pipe[2];
            pipe(my_pipe);            
            pid_t pid;
            pid=fork(); 

            if(pid<0){
                printf("Fork failed\n");
                logExit(1);
            }
            else if (pid == 0){
                initLogs();
                logCreateFork(fullpath);
                change_signal_handlers(0);
                if (getpgrp() == ParentPGID)
                    setpgid(pid, getpid()); // Child process in it's own process group
                close(my_pipe[READ]);
                int send_size = search_dir(fullpath,depth);
                logSendPipe(send_size);
                write(my_pipe[WRITE], &send_size, sizeof(long));
                logExit(0);
            }
            else{
                save_children_pid(pid);
                close(my_pipe[WRITE]);
                wait(&status);
                int subFolderSize;
                if (read(my_pipe[READ], &subFolderSize, sizeof(long))){
                    logRecvPipe(subFolderSize);
                    if(!args.separateDirs) folderSize += subFolderSize;
                }
                    
            }
        }
        save_children_pid(0);
    }
    if (depth >= -1){
        char folder_message[256];
        sprintf(folder_message,"%ld\t%s\n",folderSize,path);
        logEntry(folder_message);
        printf("%s\n", folder_message);                   //Prints the folder size if it hasn't exceeded depth
        fflush(stdout);
    } 
    
    return folderSize;
}

int main(int argc, char *argv[], char *envp[])
{
    initLogs();

    if (argc > MAX_NUM_COMMANDS || !check_args(argc, argv)) {
        fprintf(stderr, "Usage: %s -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n", argv[0]);
        exit(1);
    }

    logCreate(argc, argv); 

    ParentPGID = getpgrp();

    //printf("ARGS = {%d, %d, %d, %d, %d, %d, %d}\n", args.all, args.bytes, args.block_size, args.countLinks, args.deference, args.separateDirs, args.max_depth);

    change_signal_handlers(1);

    search_dir(args.path, args.max_depth);

    logExit(0);
}
