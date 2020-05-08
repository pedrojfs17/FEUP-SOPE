#include "utils.h"

int check_client_arg(cl_args * args, int argc, char *argv[]){
    if (argc!=4) {
        return -1;
    }

    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"-t")){
            if (i+1 == argc || args->nsecs > 0)
                return -1;
            if(atoi(argv[i+1]) > 0){
                args->nsecs=atoi(argv[i+1]);
                i++;
            }
            else{
                return -1;
            }
        }
        else if(argv[i][0]!='-'){
            strncpy(args->fifoname,argv[i],sizeof(args->fifoname));
        }
        else
            return -1;
    }

    if (args->fifoname[0] == '\0')
        return -1;

    return 0;
}

int check_server_arg(srv_args * args, int argc, char *argv[]){
    if (argc < 4 || argc > 8) {
        return -1;
    }

    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"-t")){
            if (i+1 == argc || args->nsecs > 0){
                fprintf(stderr, "Already have execution time: %d\n",args->nsecs);
                return -1;
            }
            if(atoi(argv[i+1]) > 0){
                args->nsecs=atoi(argv[i+1]);
                i++;
            }
            else{
                fprintf(stderr, "Error in secs\n");
                return -1;
            }
        }
        else if(!strcmp(argv[i],"-l")){
            if (i+1 == argc || args->nplaces > 0){
                fprintf(stderr, "ALready have places: %d\n",args->nplaces);
                return -1;
            }
            if(atoi(argv[i+1]) > 0){
                args->nplaces=atoi(argv[i+1]);
                i++;
            }
            else{
                fprintf(stderr, "Error in places\n");
                return -1;
            }
        }
        else if(!strcmp(argv[i],"-n")){
            if (i+1 == argc || args->nthreads > 0){
                fprintf(stderr, "Already have threads: %d\n",args->nthreads);
                return -1;
            }
            if(atoi(argv[i+1]) > 0){
                args->nthreads=atoi(argv[i+1]);
                i++;
            }
            else{
                fprintf(stderr, "Error in threads\n");
                return -1;
            }
        }
        else if(argv[i][0]!='-' && args->fifoname[0] == '\0'){
            strncpy(args->fifoname,argv[i],sizeof(args->fifoname));
        }
        else{
            fprintf(stderr, "Error\n");
            return -1;
        }
            
    }

    if (args->fifoname[0] == '\0'){
        fprintf(stderr, "Error in FIFO name\n");
        return -1;
    }

    return 0;
}