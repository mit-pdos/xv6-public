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

main()
{
  int pid;
  pid = fork();
  if(pid == 0){
    cons_putc('C');
  } else {
    cons_putc('P');
  }
  while(1)
    ;
}
