#include "logs.h"

struct timeval beginTime;
struct timeval currentTime;

void initClock() {
    gettimeofday(&beginTime, NULL);
}

double elapsed_time() {
    gettimeofday(&currentTime, NULL);

    double elapsed = (currentTime.tv_sec - beginTime.tv_sec) * 1e6;
    elapsed = (elapsed + (currentTime.tv_usec - beginTime.tv_usec)) * 1e-6;

    return elapsed;
}

void writeRegister(int i, int pid, long tid, int dur, int pl, char * op) {
    fprintf(stdout, "%f ; %d ; %d ; %ld ; %d ; %d ; %s\n", elapsed_time(), i, pid, tid, dur, pl, op);
    fflush(stdout);
}