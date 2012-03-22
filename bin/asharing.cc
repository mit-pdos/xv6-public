// Tests to drive abstract sharing analysis

#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "mtrace.h"
#include "pthread.h"

static int cpu;
static pthread_barrier_t bar;
enum { ncore = 8 };

void
next()
{
  if (setaffinity(cpu) < 0) {
    cpu = 0;
    if (setaffinity(cpu) < 0)
      die("sys_setaffinity(%d) failed", cpu);
  }
  cpu++;
}

void*
vmsharing(void* arg)
{
  u64 i = (u64) arg;

  volatile char *p = (char*)(0x40000UL + i * 4096);
  mtenable("xv6-mapsharing");
  if (map((void *) p, 4096) < 0)
    die("map failed");
  mtdisable("xv6-mapsharing");

  mtenable("xv6-mapsharing");
  if (unmap((void *) p, 4096) < 0)
    die("unmap failed");
  mtdisable("xv6-mapsharing");

  return 0;
}

void*
fssharing(void* arg)
{
  u64 i = (u64) arg;

  // Note that we keep these files open; otherwise all of these
  // operations will share the abstract FD object and we won't get any
  // results.

  char filename[32];
  snprintf(filename, sizeof(filename), "f%d", i);

  mtenable("xv6-fssharing");
  open(filename, O_CREATE|O_RDWR);
  mtdisable("xv6-fssharing");

  pthread_barrier_wait(&bar);

  for (u64 j = 0; j < ncore; j++) {
    snprintf(filename, sizeof(filename), "f%d", j);
    mtenable("xv6-fssharing");
    open(filename, O_RDWR);
    mtdisable("xv6-fssharing");
  }
  return 0;
}

int
main(int ac, char **av)
{
  void* (*op)(void*) = 0;
  if (ac == 2 && strcmp(av[1], "vm") == 0)
    op = vmsharing;
  else if (ac == 2 && strcmp(av[1], "fs") == 0)
    op = fssharing;
  else
    fprintf(1, "usage: %s vm|fs\n", av[0]);

  if (op) {
    pthread_barrier_init(&bar, 0, ncore);
    for (u64 i = 0; i < ncore; i++) {
      next();
      pthread_t tid;
      pthread_create(&tid, 0, op, (void*) i);
    }
  }
}
