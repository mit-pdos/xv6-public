#pragma once

#define WQSIZE 8192

class work;

int             wq_trywork(void);
int             wq_push(work *w);
void            initwq(void);
void            wq_dump(void);

struct work {
  virtual void run() = 0;
};

struct cwork : public work {
  virtual void run();

  static void* operator new(unsigned long);
  static void* operator new(unsigned long, cwork*);
  static void operator delete(void*p);

  void *rip;
  void *arg0;
  void *arg1;
  void *arg2;
  void *arg3;
  void *arg4;
};

#if defined(LINUX)
#include <stdlib.h>
#include <assert.h>
#include <atomic>
#define xmalloc(n)     malloc(n)
#define xfree(p, sz)   free(p)
#elif defined(XV6_KERNEL)
#define xmalloc(n)   kmalloc(n) 
#define xfree(p, sz) kmfree(p, sz)
#else
#warning "Unknown wq implementation"
#endif

#include "wqfor.hh"
