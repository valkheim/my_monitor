#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

# define BUFSIZE (10)
# define RES ("< ")
# define PROMPT ("------\n> ")

typedef struct s_reading
{
  char *tty;
  int baudrate;
} t_reading;

int     usage();
int     puterr(char *str);
int     is_nbr(char *n);
