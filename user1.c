void
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

void
puts(char *s)
{
  int i;

  for(i = 0; s[i]; i++)
    cons_putc(s[i]);
}

main()
{
  // fork();
  puts("hello!\n");
  while(1)
    ;
}
