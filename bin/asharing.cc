// Tests to drive abstract sharing analysis

#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "mtrace.h"
#include "pthread.h"

#include <sys/mman.h>

#include "atomic.hh"
#include <utility>

static int cpu;
std::atomic<int> barrier;
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

void
ready(void)
{
  int slot = --barrier;
  if (slot == 1) {
    mtenable_type(mtrace_record_ascope, "xv6-asharing");
    --barrier;
  } else {
    while (barrier)
      asm volatile("pause");
  }
}

u64
rnd(void)
{
  static u64 rseed;
  rseed = rseed * 6364136223846793005 + 1442695040888963407;
  return rseed;
}

void*
vmsharing(void* arg)
{
  u64 i = (u64) arg;

  ready();

  volatile char *p = (char*)(0x40000UL + i * 4096);
  if (mmap((void *) p, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) < 0)
    die("mmap failed");

  if (munmap((void *) p, 4096) < 0)
    die("munmap failed");

  return 0;
}

std::atomic<int> round;

void*
vm2sharing(void *arg)
{
  u64 i = (u64) arg;
  char *base = (char*)0x1000;

  ready();

  while (true) {
    while (round % ncore != i)
      asm volatile("pause");
    if (round >= 50) {
      round++;
      return 0;
    }

    int op = rnd() % 2;
    int lo = rnd() % 10, hi = rnd() % 10;
    if (lo > hi)
      std::swap(lo, hi);
    if (lo == hi)
      continue;

    if (op == 0) {
      // Map
      void *res = mmap(base + lo * 4096, (hi-lo) * 4096, PROT_READ|PROT_WRITE,
                       MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0);
      if (res == MAP_FAILED)
        die("asharing: mmap failed");
      // Fault
      for (int i = lo; i < hi; i++)
        base[i * 4096] = 42;
    } else {
      // Unmap
      int res = munmap(base + lo * 4096, (hi-lo) * 4096);
      if (res < 0)
        die("asharing: munmap failed");
    }

    round++;
  }
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

  open(filename, O_CREATE|O_RDWR);

  ready();

  for (u64 j = 0; j < ncore; j++) {
    snprintf(filename, sizeof(filename), "f%d", j);
    open(filename, O_RDWR);
  }
  return 0;
}

int
main(int ac, char **av)
{
  void* (*op)(void*) = 0;
  if (ac == 2 && strcmp(av[1], "vm") == 0)
    op = vmsharing;
  else if (ac == 2 && strcmp(av[1], "vm2") == 0)
    op = vm2sharing;
  else if (ac == 2 && strcmp(av[1], "fs") == 0)
    op = fssharing;
  else
    fprintf(1, "usage: %s vm|fs\n", av[0]);

  if (op) {
    barrier = ncore + 1;
    for (u64 i = 0; i < ncore; i++) {
      next();
      pthread_t tid;
      pthread_create(&tid, 0, op, (void*) i);
    }

    for (u64 i = 0; i < ncore; i++)
      wait();
    if (barrier)
      die("forgot to call ready()");
    mtdisable("xv6-asharing");
  }
}
