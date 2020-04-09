#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "args.h"

typedef enum {CREATE, EXIT, RECV_SIGNAL, SEND_SIGNAL, RECV_PIPE, SEND_PIPE, ENTRY} Action;

typedef struct {
    double instant;
    pid_t pid;
    Action action;
    char info[512];
} Register;

const char* getAction(Action action);

const char* getSignal(int signo);

void initLogs();

double elapsed_time();

void createRegister(Register * reg, Action action);

void writeRegister(Register * reg);

//------------------------------------------------------------------------------------------

void logCreate(int argc, char *argv[]);

void logCreateFork(char * path, struct Args args, int maxdepth);

void logExit(int exitStatus);

void logRecvSignal(int sig);

void logSendSignal(pid_t pid, int sig);

void logRecvPipe(int size);

void logSendPipe(int size);

void logEntry(char * message);

