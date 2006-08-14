#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"

int
puts(char *s)
{
  return write(1, s, strlen(s));
}

char*
strcpy(char *s, char *t)
{
	char *os;
	
	os = s;
	while((*s++ = *t++) != 0)
		;
	return os;
}

unsigned int
strlen(char *s)
{
  int n = 0;
  for(n = 0; s[n]; n++)
    ;
  return n;
}

void *
memset(void *dst, int c, unsigned int n)
{
  char *d = (char *) dst;

  while(n-- > 0)
    *d++ = c;

  return dst;
}

char *
gets(char *buf, int max)
{
  int i = 0, cc;
  char c;
  
  while(i+1 < max){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    if(c == '\n' || c == '\r')
      break;
    buf[i++] = c;
  }
  buf[i] = '\0';
  return buf;
}

int
stat(char *n, struct stat *st)
{
  int fd = open(n, O_RDONLY);
  int r;

  if (fd < 0) return -1;

  r = fstat(fd, st);
  close(fd);
  return r;
}
