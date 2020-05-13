#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    unsigned int nsecs;
    char fifoname[255];
} cl_args;

typedef struct{
    unsigned int nsecs;
    unsigned int nplaces;
    unsigned int nthreads;
    char fifoname[255];
} srv_args;

int check_client_arg(cl_args * args, int argc, char *argv[]);
int check_server_arg(srv_args * args, int argc, char *argv[]);

int read_public_message(int fd, char* msg);