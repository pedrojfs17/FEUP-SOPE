#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

#define IWANT "IWANT"
#define RECEIVED "RECVD"
#define ENTER "ENTER"
#define IAMIN "IAMIN"
#define TIMEUP "TIMUP"
#define TOLATE "2LATE"
#define CLOSED "CLOSD"
#define FAILED "FAILD"
#define GAVEUP "GAVUP"

void initClock();

double elapsed_time();

void writeRegister(int i, int pid, long tid, int dur, int pl, char * op);
