int
fork()
{
  asm("mov $1, %eax");
  asm("int $48");
}

void
cons_putc(int c)
{
  asm("mov $4, %eax");
  asm("int $48");
}

int
puts(char *s)
{
  int i;

  for(i = 0; s[i]; i++)
    cons_putc(s[i]);
  return i;
}

int
pipe(int fds[])
{
  asm("mov $5, %eax");
  asm("int $48");
}

int
read(int fd, char *buf, int n)
{
  asm("mov $7, %eax");
  asm("int $48");
}

int
write(int fd, char *buf, int n)
{
  asm("mov $6, %eax");
  asm("int $48");
}
