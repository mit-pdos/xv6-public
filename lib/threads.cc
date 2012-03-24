#include "types.h"
#include "pthread.h"
#include "user.h"
#include "atomic.hh"
#include "fcntl.h"

enum { stack_size = 8192 };
static std::atomic<int> nextkey;

struct tlsdata {
  void* buf[128];
};

void
forkt_setup(u64 pid)
{
  tlsdata* t = (tlsdata*) sbrk(sizeof(*t));
  setfs((u64) t);
}

int
pthread_create(pthread_t* tid, const pthread_attr_t* attr,
               void* (*start)(void*), void* arg)
{
  char* base = (char*) sbrk(stack_size);
  int t = forkt(base + stack_size, (void*) start, arg, FORK_SHARE_VMAP);
  if (t < 0)
    return t;

  *tid = t;
  return 0;
}

pthread_t
pthread_self()
{
  return getpid();
}

int
pthread_key_create(pthread_key_t *key, void (*destructor)(void*))
{
  // Ignore the destructor for now.
  *key = nextkey++;
  return 0;
}

void*
pthread_getspecific(pthread_key_t key)
{
  u64 v;
  __asm volatile("movq %%fs:(%1), %0" : "=r" (v) : "r" ((u64) key * 8));
  return (void*) v;
}

int
pthread_setspecific(pthread_key_t key, void* value)
{
  __asm volatile("movq %0, %%fs:(%1)" : : "r" (value), "r" ((u64) key * 8) : "memory");
  return 0;
}

int
pthread_barrier_init(pthread_barrier_t *b,
                     const pthread_barrierattr_t *attr, unsigned count)
{
  b->store(count);
  return 0;
}

int
pthread_barrier_wait(pthread_barrier_t *b)
{
  (*b)--;
  while (*b != 0)
    ;   // spin
  return 0;
}
