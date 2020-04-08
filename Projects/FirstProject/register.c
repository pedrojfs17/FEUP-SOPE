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

const char* getSignal(int signo) {
   switch (signo) 
   {
      case SIGINT: return "SIGINT";
      case SIGSTOP: return "SIGSTOP";
      case SIGCONT: return "SIGCONT";
      case SIGTERM: return "SIGTERM";
      default: return "SIG";
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
    fflush(logFile);
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

void logCreateFork(char * path, struct Args args, int maxdepth) {
    Register reg; createRegister(&reg, CREATE);

    char temp[64];

    strcat(reg.info, "./simpledu ");
    strcat(reg.info, path);
    if (args.all)
        strcat(reg.info, " -a");
    if (args.bytes)
        strcat(reg.info, " -b");
    if (args.countLinks)
        strcat(reg.info, " -l");
    if (args.deference)
        strcat(reg.info, " -L");
    if (args.separateDirs)
        strcat(reg.info, " -S");
    if (args.block_size != 1024) {
        strcat(reg.info, " -B ");
        sprintf(temp, "%d", args.block_size);
        strcat(reg.info, temp);
    }
    if (args.max_depth != __INT_MAX__) {
        strcat(reg.info, " --max-depth=");
        sprintf(temp, "%d", maxdepth);
        strcat(reg.info, temp);
    }

    writeRegister(&reg);
}

void logExit(int exitStatus) {
    Register reg; createRegister(&reg, EXIT);

    sprintf(reg.info, "%d", exitStatus);

    writeRegister(&reg);

    exit(exitStatus);
}

void logRecvPipe(int size){
    Register reg; createRegister(&reg, RECV_PIPE);
    sprintf(reg.info, "%d",size);
    writeRegister(&reg);

}

void logSendPipe(int size){
    Register reg; createRegister(&reg, SEND_PIPE);
    sprintf(reg.info, "%d",size);
    writeRegister(&reg);

}

void logEntry(char * message){
    Register reg; createRegister(&reg, ENTRY);
    char * tok=strtok(message,"\n");
    sprintf(reg.info, "%s",tok);
    writeRegister(&reg);
}

void logRecvSignal(int sig) {
    Register reg; createRegister(&reg, RECV_SIGNAL);

    sprintf(reg.info, "%s", getSignal(sig));

    writeRegister(&reg);
}

void logSendSignal(pid_t pid, int sig) {
    Register reg; createRegister(&reg, SEND_SIGNAL);

    sprintf(reg.info, "Sent %s to process %d", getSignal(sig), pid);

    writeRegister(&reg);
}

