#include "types.h"
#include "stat.h"
#include "user.h"

#include <stdarg.h>

static void
printint(void (*putch) (void*, char), void *putarg,
	 s64 xx, int base, int sgn)
{
  const static char digits[] = "0123456789ABCDEF";
  char buf[21];
  int i, neg;
  s64 x;

  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);
  if(neg)
    buf[i++] = '-';

  while(--i >= 0)
    putch(putarg, buf[i]);
}

// Only understands %d, %x, %p, %s.
void
vprintfmt(void (*putch) (void*, char), void *putarg,
          const char *fmt, va_list ap)
{
  const char *s;
  int c, i, state;

  state = 0;
  for(i = 0; fmt[i]; i++){
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
        state = '%';
      } else {
        putch(putarg, c);
      }
    } else if(state == '%'){
      if(c == 'd'){
        printint(putch, putarg, va_arg(ap, u32), 10, 1);
      } else if(c == 'x') {
        printint(putch, putarg, va_arg(ap, u32), 16, 0);
      } else if(c == 'l') {
        state = 'l';
        continue;
      } else if(c == 's'){
        s = (const char*) va_arg(ap, const char*);
        if(s == 0)
          s = "(null)";
        while(*s != 0){
          putch(putarg, *s);
          s++;
        }
      } else if(c == 'c'){
        putch(putarg, va_arg(ap, u32));
      } else if(c == '%'){
        putch(putarg, c);
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putch(putarg, '%');
        putch(putarg, c);
      }
      state = 0;
    } else if(state == 'l') {
      if(c == 'x') {
        printint(putch, putarg, va_arg(ap, u64), 16, 0);
      }
      else if(c == 'u') {
        printint(putch, putarg, va_arg(ap, u64), 10, 0);
      }
      else {
        // Unknown % sequence.  Print it to draw attention.
        putch(putarg, '%');
        putch(putarg, c);
      }
      state = 0;
    }
  }
}

// Print to the given fd.
static void
writec(void *arg, char c)
{
    int fd = (int) (u64) arg;
  write(fd, &c, 1);
}

void
printf(int fd, const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintfmt(writec, (void*) (u64)fd, fmt, ap);
  va_end(ap);
}

// Print to a buffer.
struct bufstate {
  char *p;
  char *e;
};

static void
writebuf(void *arg, char c)
{
  struct bufstate *bs = (bufstate*) arg;
  if (bs->p < bs->e) {
    bs->p[0] = c;
    bs->p++;
  }
}

void
vsnprintf(char *buf, u32 n, const char *fmt, va_list ap)
{
  struct bufstate bs = { buf, buf+n-1 };
  vprintfmt(writebuf, (void*) &bs, fmt, ap);
  bs.p[0] = '\0';
}

void
snprintf(char *buf, u32 n, const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(buf, n, fmt, ap);
  va_end(ap);
}

void __attribute__((noreturn))
die(const char* errstr, ...)
{
  va_list ap;

  va_start(ap, errstr);
  vprintfmt(writec, (void*) (u64)1, errstr, ap);
  va_end(ap);
  printf(1, "\n");
  exit();
}
