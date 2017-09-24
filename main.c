#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include "my_monitor.h"

void     *writer(void *i)
{
  int fd = *((int *) i);
  char line[BUFSIZE];

  free(i);
  //write(1, PROMPT, strlen(PROMPT));
  //while (fgets(line, BUFSIZE, stdin))
  while (read(0, line, BUFSIZE))
  {
    line[BUFSIZE] = 0;
    //line[strcspn(line, "\n")] = '\0'; //remove fgets trailing newline
    //write(1, RES, strlen(RES));
    write(fd, line, BUFSIZE);
    memset(line, 0, BUFSIZE);
    fflush(stdin);
    //write(1, PROMPT, strlen(PROMPT));
  }
  return NULL;
}

void reader(char *tty, int baudrate)
{
  int   fd;
  pthread_t writer_th;
  int *writer_fd;
  struct termios new;
  struct termios old;

  if ((fd = open(tty, O_RDWR | O_NOCTTY)) < 0)
    pthread_exit(NULL);

  tcgetattr(fd, &old);
  tcgetattr(fd, &new);

  new.c_iflag = IGNBRK;
  new.c_oflag = 0L;
  new.c_lflag = 0L;
  new.c_cc[VMIN] = 1;
  new.c_cc[VTIME] = 0;
  new.c_cflag |= CREAD;
  new.c_cflag &=~ (CSIZE|PARENB|CSTOPB|CRTSCTS);
  new.c_cflag |= CS8;
  new.c_cflag |= CLOCAL;
  new.c_iflag &=~ (IXON|IXOFF|IXANY);

  cfsetispeed(&new, baudrate);
  cfsetospeed(&new, baudrate);
  tcsetattr(fd, TCSANOW, &new);
  tcflush(fd, TCIFLUSH);

  writer_fd = malloc(sizeof(*writer_fd));
  *writer_fd = fd;
  if (pthread_create(&writer_th, NULL, writer, (void*)writer_fd))
    perror("writer_th create");
  char line[BUFSIZE];
  //while (read(fd, &c, 1))
  while (read(fd, line, BUFSIZE))
  {
    line[BUFSIZE] = 0;
    write(1, ".", 1);
    write(1, line, BUFSIZE);
    fflush(stdout);
  }
  if (pthread_join(writer_th, NULL))
    perror("writer_th join");
  tcsetattr(fd, TCSANOW, &old);
  close(fd);
}

int     main(int ac, char **av)
{
  if (ac != 3)
    return (usage());
  if (access(av[1], F_OK) == -1)
    return (puterr(strerror(errno)));
  if (is_nbr(av[2]))
    return (puterr("Invalid baudrate."));
  reader(av[1], atoi(av[2]));
}
