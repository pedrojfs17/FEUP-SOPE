#include "register.h"

clock_t beginTime;
FILE * logFile;

const char* getAction(Action action) {
   switch (action) 
   {
      case CREATE: return "CREATE";
      case EXIT: return "EXIT";
      case RECV_SIGNAL: return "RECV_SIGNAL";
      case SEND_SIGNAL: return "SEND_SIGNAL";
      case RECV_PIPE: return "RECV_PIPE";
      case SEND_PIPE: return "SEND_PIPE";
      case ENTRY: return "ENTRY";
      default: return "CREATE";
   }
}

void initLogs() {
    beginTime = clock();

    setenv("LOG_FILENAME", "log.txt", 0);

    logFile = fopen(getenv("LOG_FILENAME"), "a");
    if(logFile == NULL) {
        perror("Error opening file");
        exit(1);
    }
}

void createRegister(Register * reg, Action action) {
    clock_t currentTime = clock();

    reg->instant = (currentTime - beginTime)/(CLOCKS_PER_SEC / (double) 1000.0);
    reg->pid = getpid();
    reg->action = action;
    strncpy(reg->info,"Command: ", sizeof("Command: "));
}

void writeRegister(Register * reg) {
    fprintf(logFile, "%.2f - %.8d - %s - %s\n", reg->instant, reg->pid, getAction(reg->action), reg->info);
}

void logCreate(int argc, char *argv[]) {
    Register reg; createRegister(&reg, CREATE);

    for(int i = 0; i < argc; i++){
		strcat(reg.info, argv[i]);
        if (i != argc-1)
            strcat(reg.info," ");
	}

    writeRegister(&reg);
}

void logExit(int exitStatus) {
    Register reg; createRegister(&reg, EXIT);

    sprintf(reg.info, "%d", exitStatus);

    writeRegister(&reg);

    exit(exitStatus);
}

void logRecvSignal(int sig) {
    Register reg; createRegister(&reg, RECV_SIGNAL);

    sprintf(reg.info, "%d", sig);

    writeRegister(&reg);
}

void logSendSignal(pid_t pid, int sig) {
    Register reg; createRegister(&reg, SEND_SIGNAL);

    sprintf(reg.info, "Sent %d to process %d", sig, pid);

    writeRegister(&reg);
}

