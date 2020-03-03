//FOLHA 2 - p4a.c

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define BUFFER_SIZE 512

int main(int argc, char *argv[])
{
  int fd;
  unsigned char buffer[BUFFER_SIZE];

  if (argc != 2) {
    printf("Usage: %s <destination>\n", argv[0]);
    return 1;
  }

  fd=open(argv[1], O_WRONLY | O_CREAT | O_EXCL, 0644);
  if (fd == -1) {
    perror(argv[1]);
    return 2;
  }

  while (*buffer != '\t') {
    write(STDOUT_FILENO, "Name: ", 7);
    while (read(STDIN_FILENO, buffer, 1) && *buffer != '\n') {
      write(fd, buffer, 1);
    }
    write(fd, " - ", 4);
    write(STDOUT_FILENO, "Grade: ", 8);
    while (read(STDIN_FILENO, buffer, 1) && *buffer != '\n') {
      write(fd, buffer, 1);
    }
    write(fd, "\n", 2);
    write(STDOUT_FILENO, "Student added! TAB + Enter to Exit or Enter to add another. ",55);
    if (read(STDIN_FILENO, buffer, 1) && *buffer == '\t')
      break;
  }

  write(STDOUT_FILENO, "FINISHED!\n",11);
  close(fd);

  return 0;
}
