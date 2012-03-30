#pragma once

class work;

int             wq_trywork(void);
int             wq_push(work *w);
int             wq_pushto(work *w, int tcpuid);
void            wq_dump(void);
size_t          wq_size(void);
void            initwq(void);
void            exitwq(void);

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
#define xallocwork(n)    malloc(n)
#define xfreework(p, sz) free(p)
#elif defined(XV6_KERNEL)
#define xallocwork(n)    kmalloc(n, "xallocwork")
#define xfreework(p, sz) kmfree(p, sz)
#else
extern void* allocwork(unsigned long nbytes);
extern void  freework(void *ptr);
#define xallocwork(n)    allocwork(n)
#define xfreework(n, sz) freework(n)
#endif

#include "wqfor.hh"
