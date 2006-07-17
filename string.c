#include "types.h"
#include "defs.h"

void *
memset(void *dst, int c, uint n)
{
  char *d = (char *) dst;

  while(n-- > 0)
    *d++ = c;

  return dst;
}

int
memcmp(const void *v1, const void *v2, uint n)
{
  const uint8_t *s1 = (const uint8_t *) v1;
  const uint8_t *s2 = (const uint8_t *) v2;

  while (n-- > 0) {
    if (*s1 != *s2)
      return (int) *s1 - (int) *s2;
    s1++, s2++;
  }

  return 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const char *s;
  char *d;
	
  s = src;
  d = dst;
  if (s < d && s + n > d) {
    s += n;
    d += n;
    while (n-- > 0)
      *--d = *--s;
  } else
    while (n-- > 0)
      *d++ = *s++;

  return dst;
}

int
strncmp(const char *p, const char *q, uint n)
{
	while (n > 0 && *p && *p == *q)
		n--, p++, q++;
	if (n == 0)
		return 0;
	else
		return (int) ((uint8_t) *p - (uint8_t) *q);
}

// Memcpy is deprecated and should NOT be called.
// Use memmove instead, which has defined semantics
// when the two memory ranges overlap.
// Memcpy is here only because gcc compiles some
// structure assignments into calls to memcpy.
void *
memcpy(void *dst, void *src, uint n)
{
  char *d = (char *) dst;
  char *s = (char *) src;

  while(n-- > 0)
    *d++ = *s++;

  return dst;
}

