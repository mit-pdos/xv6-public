#include "types.h"
#include "defs.h"

void*
memset(void *dst, int c, uint n)
{
  char *d = (char*) dst;

  while(n-- > 0)
    *d++ = c;

  return dst;
}

int
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1 = (const uchar*) v1;
  const uchar *s2 = (const uchar*) v2;

  while(n-- > 0) {
    if(*s1 != *s2)
      return (int) *s1 - (int) *s2;
    s1++, s2++;
  }

  return 0;
}

void*
memmove(void *dst, const void *src, uint n)
{
  const char *s;
  char *d;

  s = src;
  d = dst;
  if(s < d && s + n > d) {
    s += n;
    d += n;
    while(n-- > 0)
      *--d = *--s;
  } else
    while(n-- > 0)
      *d++ = *s++;

  return dst;
}

int
strncmp(const char *p, const char *q, uint n)
{
  while(n > 0 && *p && *p == *q)
    n--, p++, q++;
  if(n == 0)
    return 0;
  else
    return (int) ((uchar) *p - (uchar) *q);
}
