#include "types.h"
#include "pthread.h"
#include "user.h"
#include "atomic.hh"

enum { stack_size = 8192 };
static std::atomic<int> nextkey;

struct tlsdata {
  void* buf[128];
};

void
forkt_setup()
{
  printf("forkt_setup: pid %d\n", getpid());

  tlsdata* t = (tlsdata*) sbrk(sizeof(*t));
  setfs((u64) t);
}

int
pthread_create(pthread_t* tid, const pthread_attr_t* attr,
               void* (*start)(void*), void* arg)
{
  char* base = (char*) sbrk(stack_size);
  int t = forkt(base + stack_size, (void*) start, arg);
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
  __asm volatile("movq %%fs:(%1), %0" : "=r" (v) : "r" (key * 8));
  return (void*) v;
}

int
pthread_setspecific(pthread_key_t key, void* value)
{
  __asm volatile("movq %0, %%fs:(%1)" : : "r" (value), "r" (key * 8));
  return 0;
}
