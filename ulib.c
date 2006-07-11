int
puts(char *s)
{
  int i;

  for(i = 0; s[i]; i++)
    cons_putc(s[i]);
  return i;
}

