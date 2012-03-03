#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <sched.h>

#define __noret__ __attribute__((noreturn))

static inline __noret__ void 
die(const char* errstr, ...) 
{
  va_list ap;

  va_start(ap, errstr);
  vfprintf(stderr, errstr, ap);
  va_end(ap);
  fprintf(stderr, "\n");
  exit(EXIT_FAILURE);
}

static inline __noret__ void 
edie(const char* errstr, ...) 
{
  va_list ap;

  va_start(ap, errstr);
  vfprintf(stderr, errstr, ap);
  va_end(ap);
  fprintf(stderr, ": %s\n", strerror(errno));
  exit(EXIT_FAILURE);
}

static inline void
setaffinity(int c)
{
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(c, &cpuset);
  if (sched_setaffinity(0, sizeof(cpuset), &cpuset) < 0)
    edie("setaffinity, sched_setaffinity failed");
}
