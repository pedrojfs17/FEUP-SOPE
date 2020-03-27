#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

typedef enum {CREATE, EXIT, RECV_SIGNAL, SEND_SIGNAL, RECV_PIPE, SEND_PIPE, ENTRY} Action;

typedef struct {
    double instant;
    pid_t pid;
    Action action;
    char info[256];
} Register;

const char* getAction(Action action);

void initLogs();

void createRegister(Register * reg, Action action);

void writeRegister(Register * reg);

//------------------------------------------------------------------------------------------

void logCreate(int argc, char *argv[]);

void logExit(int exitStatus);

void logRecvSignal(int sig);

void logSendSignal(pid_t pid, int sig);

void logRecvPipe();

void logSendPipe();

void logEntry();

