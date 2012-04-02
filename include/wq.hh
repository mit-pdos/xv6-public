#pragma once

#include "wqtypes.hh"
#include "percpu.hh"

struct uwq_ipcbuf;
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

struct wframe {
  wframe(int v = 0) : v_(v) {}
  void clear() { v_ = 0; }
  int inc() { return __sync_add_and_fetch(&v_, 1); }
  int dec() { return __sync_sub_and_fetch(&v_, 1); }
  bool zero() volatile { return v_ == 0; };
  volatile int v_;
};

#define NSLOTS (1 << WQSHIFT)

class wq {
public:
  wq();
  int push(work *w, int tcpuid);
  int trywork();
  void dump();

  static void* operator new(unsigned long);
  static void operator delete(void*);

private:
  work *steal(int c);
  work *pop(int c);
  void inclen(int c);
  void declen(int c);

  struct wqueue {
    work *w[NSLOTS];
    volatile int head __mpalign__;
    volatile int tail;
    wqlock_t lock;
  };

  struct stat {
    u64 push;
    u64 full;
    u64 pop;
    u64 steal;
  };

  percpu<wqueue> q_;
  percpu<stat> stat_;

#if defined(XV6_USER)
  uwq_ipcbuf* ipc_;
#endif
};

void* xallocwork(unsigned long nbytes);
void  xfreework(void* ptr, unsigned long nbytes);

#if defined(XV6_USER)
void* wqalloc(unsigned long nbytes);
void  wqfree(void *ptr);
extern u64 wq_maxworkers;
#endif

#if defined(LINUX)
#include <assert.h>
extern u64 wq_maxworkers;
#endif

#include "wqfor.hh"
