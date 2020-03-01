//FOLHA 2 - p2b.c
//FILE COPY
//USAGE: copy source destination

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define BUFFER_SIZE 512

int main(int argc, char *argv[])
{
  int fd1, fd2, nr;
  unsigned char buffer[BUFFER_SIZE];

  if (argc != 2 && argc != 3) {
    printf("Usage: %s <source> <destination>\n", argv[0]);
    return 1;
  }

  fd1 = open(argv[1], O_RDONLY);
  if (fd1 == -1) {
    perror(argv[1]);
    return 2;
  }

  if (argc == 3) {
    fd2 = open(argv[2], O_WRONLY);
    if (fd2 == -1) {
      perror(argv[2]);
      close(fd1);
      return 3;
    }
    dup2(fd2, 1);
  }

  while ((nr = read(fd1, buffer, 1)) > 0)
    printf("%s",buffer);

  close(fd1);
  if (argc == 3)
    close(fd2);

  return 0;
}
