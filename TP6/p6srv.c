#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

#define MAX_NAME_LEN 20

int main(void)
{
    int fd;//, fd_dummy;
    char name[MAX_NAME_LEN];
    int num[2];

    if (mkfifo("./fifo_req",0660)<0)
        if (errno == EEXIST) printf("FIFO './fifo_req' already exists\n");
        else printf("Can't create FIFO\n");
    else 
        printf("FIFO './fifo_req' sucessfully created\n");
    
    if ((fd=open("./fifo_req",O_RDONLY)) !=-1)
        printf("FIFO './fifo_req' openned in READONLY mode\n");
    
    /*if ((fd_dummy=open("./fifo_req",O_WRONLY)) !=-1)
        printf("FIFO './fifo_req' openned in WRITEONLY mode\n");*/

    do {
        read(fd,&num[0],sizeof(int));
        read(fd,&num[1],sizeof(int));
        if (num[0]!=0 || num[1]!=0) {
            read(fd,&name,MAX_NAME_LEN);
            printf("Operands: %d e %d. FIFO name = %s\n",num[0],num[1],name);
        }
    } while (num[0]!=0 || num[1]!=0);

    close(fd);
    //close(fd_dummy);

    if (unlink("./fifo_req")<0)
        printf("Error when destroying FIFO './fifo_req'\n");
    else
        printf("FIFO './fifo_req' has been destroyed\n");
    
    exit(0);
} 