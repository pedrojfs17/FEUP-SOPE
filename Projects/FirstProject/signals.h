#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>

#include "register.h"

void change_signal_handlers(int parent);

void sigint_handler(int signo);

void sigtstp_handler(int signo);

void sigterm_handler(int signo);

void sigcont_handler(int signo);

void save_children_pid(pid_t pid);
