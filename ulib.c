#include "user.h"

int
puts(char *s)
{
  return cons_puts(s);
}

int
puts1(char *s)
{
  int i;

  for(i = 0; s[i]; i++)
    cons_putc(s[i]);
  return i;
}

