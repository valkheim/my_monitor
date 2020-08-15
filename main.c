#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

int usage()
{
  puts("Usage: ./reader [interface] [baudrate]");
  return 2;
}

int is_nbr(char const * n)
{
  while (*n)
  {
    if (!isdigit(*n))
      return EXIT_FAILURE;

    n++;
  }
  return EXIT_SUCCESS;
}

int reader(char *tty, int baudrate)
{
  int fd;
  char c;
  struct termios new;
  struct termios old;

  if ((fd = open(tty, O_RDWR | O_NOCTTY)) < 0)
    return EXIT_FAILURE;

  if(tcgetattr(fd, &old) != 0)
    printf("Error %i getting old tcgetattr: %s\n", errno, strerror(errno));

  if(tcgetattr(fd, &new) != 0)
    printf("Error %i getting new tcgetattr: %s\n", errno, strerror(errno));

  new.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
  new.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
  new.c_cflag |= CS8; // 8 bits per byte
  new.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
  new.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

  new.c_lflag &= ICANON; // Receive line by line
  new.c_lflag &= ~ECHO; // Disable echo
  new.c_lflag &= ~ECHOE; // Disable erasure
  new.c_lflag &= ~ECHONL; // Disable new-line echo
  new.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
  new.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
  new.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

  new.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
  new.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

  new.c_cc[VTIME] = 10; // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
  new.c_cc[VMIN] = 0;

  cfsetispeed(&new, baudrate);
  cfsetospeed(&new, baudrate);

  if (tcsetattr(fd, TCSANOW, &new) != 0)
    printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));

  tcflush(fd, TCIFLUSH);

  while (read(fd, &c, 1))
    write(1, &c, 1);

  tcsetattr(fd, TCSANOW, &old);
  close(fd);
  return EXIT_SUCCESS;
}

int     main(int ac, char **av)
{
  if (ac != 3)
    return usage();

  if (access(av[1], F_OK) == -1)
    return fprintf(stderr, strerror(errno));

  if (is_nbr(av[2]))
    return fprintf(stderr, "Invalid baudrate.");

  return reader(av[1], atoi(av[2]));
}
