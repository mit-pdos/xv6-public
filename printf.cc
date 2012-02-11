#include "types.h"
#include "stat.h"
#include "user.h"
extern "C" {
#include <stdarg.h>
#include "fmt.h"
}

// Print to the given fd.
static void
writec(int c, void *arg)
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
writebuf(int c, void *arg)
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
