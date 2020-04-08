#include "signals.h"

pid_t childrenPid = 0;

void change_signal_handlers(int parent) {
    if (parent) {
        // Change SIGINT handler
        signal(SIGINT,sigint_handler);
    }
    else {
        // Change SIGTERM handler
        signal(SIGTERM,sigterm_handler);
        // Change SIGCONT handler
        signal(SIGCONT,sigcont_handler);
    }
}

void sigint_handler(int signo) {
    logRecvSignal(signo);
    if (childrenPid) {
        logSendSignal(childrenPid, SIGSTOP);
        kill(-childrenPid, SIGSTOP);
    }

    char terminate;

    do {
        if (terminate == '\0')
            printf("\nDo you want to terminate? (Y/N): ");
        fflush(stdout);
        scanf("%c",&terminate);
        if(terminate == 'Y' || terminate == 'y'){
            if (childrenPid) {
                logSendSignal(childrenPid, SIGTERM);
                kill(childrenPid, SIGTERM);
                logSendSignal(childrenPid, SIGCONT);
                kill(childrenPid, SIGCONT); // Precisa deste sinal para poder continuar a correr o codigo do handler sigterm
                waitpid(-childrenPid, NULL, 0);
            }
            logExit(1);
        }
        else if(terminate == 'N' || terminate == 'n'){
            if (childrenPid) {
                logSendSignal(childrenPid, SIGCONT);
                kill(childrenPid, SIGCONT); 
            }
        }
        else if (terminate != '\n'){
            printf("Invalid input! Try again: ");
        }
    } while (terminate != 'Y' && terminate != 'y' && terminate != 'N' && terminate != 'n');
    terminate = '\0';
} 

void sigterm_handler(int signo) {
    logRecvSignal(signo);
    if (childrenPid) {
        logSendSignal(childrenPid, SIGTERM);
        kill(childrenPid, SIGTERM);
    }
    logExit(1);
}

void sigcont_handler(int signo) {
    logRecvSignal(signo);
    if (childrenPid) {
        logSendSignal(childrenPid, SIGCONT);
        kill(childrenPid, SIGCONT);
    }
}

void save_children_pid(pid_t pid) {
    childrenPid = pid;
}