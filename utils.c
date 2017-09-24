#include "my_monitor.h"

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

