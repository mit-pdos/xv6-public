#include "types.h"
#include "defs.h"

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

int
memcmp(const void *v1, const void *v2, unsigned n)
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
