void *
memcpy(void *dst, void *src, unsigned n)
{
  char *d = (char *) dst;
  char *s = (char *) src;

  while(n-- > 0)
    *d++ = *s++;

  return dst;
}

void *
memset(void *dst, int c, unsigned n)
{
  char *d = (char *) dst;

  while(n-- > 0)
    *d++ = c;

  return dst;
}
