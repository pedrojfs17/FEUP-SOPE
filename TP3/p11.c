#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define COMMAND_SIZE 200
#define MAX_NUM_COMMANDS 20

void get_tokens(char* tokens[MAX_NUM_COMMANDS] , char command[COMMAND_SIZE] , int* numCommands) {
	const char delim[] = " ";
	char* command_token;
	*numCommands = 0;
	
	command_token = strtok(command , delim);
	
	while(command_token != NULL){
	    tokens[(*numCommands)] = command_token;
	    (*numCommands)++;
	    command_token = strtok(NULL , delim);
	}	
}

void checkOutputRedirection(char* tokens[MAX_NUM_COMMANDS] , int numCommands) {
    if (numCommands < 3 || strcmp(tokens[numCommands-2] , "-o")!=0) {
        return;
    }
    
    // Open file
    int fd = open(tokens[numCommands-1] , O_WRONLY | O_CREAT | O_TRUNC , 00644);
    
    if (fd == -1) {
        write(STDOUT_FILENO , "\n\nFailed to open file. Writing to STDOUT instead.\n" , 50);
        return;
    }
    
    dup2(fd , STDOUT_FILENO);
    
    tokens[numCommands-2] = NULL;
    
    return;
}

int main(void)
{
    int pid, pid_terminated, status;

    char command[COMMAND_SIZE];
    char* tokens[MAX_NUM_COMMANDS];
    int numCommands;

    printf("minish > "); 
    scanf ("%[^\n]%*c", command);

    while (strcmp(command,"quit") != 0) {
        pid=fork();
        if (pid > 0) { // COMMENT THE 2 LINES BELOW TO SEE THE ZOMBIES
            pid_terminated = wait(&status);
            printf("PARENT: son %d terminated with exit code %d\n",
            pid_terminated,WEXITSTATUS(status));
        }
        else {
            get_tokens(tokens, command, &numCommands);
            tokens[numCommands] = NULL;
            checkOutputRedirection(tokens , numCommands);
            execvp(tokens[0],tokens);
            printf("Command not found !!!\n");
            exit(1);
        }

        printf("minish > ");
        scanf ("%[^\n]%*c", command);
    }

    return 0;
}