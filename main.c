#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>

int     usage()
{
  puts("Usage: ./reader [interface] [baudrate]");
  return (EXIT_SUCCESS);
}

int     puterr(char *str)
{
  write(1, str, strlen(str));
  return (EXIT_FAILURE);
}

int     is_nbr(char *n)
{
  int   i;

  i = -1;
  while (n[++i]) {
    if (n[i] < '0' ||
        n[i] > '9')
      return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

int     reader(char *tty, int baudrate)
{
  int   fd;
  char  c;
  struct termios new;
  struct termios old;

  if ((fd = open(tty, O_RDWR | O_NOCTTY)) < 0)
    return (EXIT_FAILURE);

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

  while (read(fd, &c, 1)) {
    write(1, &c, 1);
  }

  tcsetattr(fd, TCSANOW, &old);
  close(fd);

  return (EXIT_SUCCESS);
}

int     main(int ac, char **av)
{
  if (ac != 3)
    return (usage());
  if (access(av[1], F_OK) == -1)
    return (puterr(strerror(errno)));
  if (is_nbr(av[2]))
    return (puterr("Invalid baudrate."));
  return (reader(av[1], atoi(av[2])));
}
