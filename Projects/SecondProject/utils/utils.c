#include "utils.h"

int check_client_arg(cl_args * args, int argc, char *argv[]){
    if (argc!=4) {
        printf("Usage: U1 <-t secs> fifoname\n");
        return -1;
    }

    for(int i=0;i<argc;i++){
        if(!strcmp(argv[i],"-t")){
            if(atoi(argv[i+1])){
                args->nsecs=atoi(argv[i+1]);
                i++;
            }
            else{
                return -1;
            }
        }
        else if(argv[i][1]!='-'){
            strncpy(args->fifoname,argv[i],sizeof(args->fifoname));
        }
    }
    return 0;
}

int check_server_arg(srv_args * args, int argc, char *argv[]){
    if (argc>8) {
        printf("Usage: Q1 <-t secs> fifoname\n");
        return -1;
    }

    for(int i=0;i<argc;i++){
        if(!strcmp(argv[i],"-t")){
            if(atoi(argv[i+1])){
                args->nsecs=atoi(argv[i+1]);
                i++;
            }
            else{
                printf("Error in secs\n");
                return -1;
            }
        }
        else if(!strcmp(argv[i],"-l")){
            if(atoi(argv[i+1])){
                args->nplaces=atoi(argv[i+1]);
                i++;
            }
            else{
                printf("Error in places\n");
                return -1;
            }
        }
        else if(!strcmp(argv[i],"-n")){
            if(atoi(argv[i+1])){
                args->nthreads=atoi(argv[i+1]);
                i++;
            }
            else{
                printf("Error in threads\n");
                return -1;
            }
        }
        else if(argv[i][1]!='-'){
            strncpy(args->fifoname,argv[i],sizeof(args->fifoname));
        }
    }

    return 0;
}