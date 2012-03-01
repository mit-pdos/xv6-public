#include "types.h"
#include "kernel.hh"
#include "spinlock.h"
#include "amd64.h"
#include "cpu.hh"
#include "wq.hh"

#define NSLOTS (1 << WQSHIFT)

struct wqueue {
  struct work *w[NSLOTS];
  
  volatile int head __mpalign__;
  volatile int tail;
  struct spinlock lock;
  __padout__;
} __mpalign__;;

struct wqstat {
  u64 push;
  u64 full;
  u64 pop;
  u64 steal;
  __padout__;
} __mpalign__;

struct wqueue queue[NCPU] __mpalign__;
struct wqstat stat[NCPU] __mpalign__;

static inline struct wqueue *
getwq(void)
{
  pushcli();
  return &queue[mycpu()->id];
}

static inline void
putwq(struct wqueue *wq)
{
  popcli();
}

static inline struct wqstat *
wq_stat(void)
{
  return &stat[mycpu()->id];
}

struct work *
allocwork(void)
{
  return (struct work *)kalloc();
}

void
freework(struct work *w)
{
  kfree(w);
}

int
wq_push(struct work *w)
{
  int i;

  struct wqueue *wq = getwq();
  i = wq->head;
  if ((i - wq->tail) == NSLOTS) {
    wq_stat()->full++;
    return -1;
  }
  i = i & (NSLOTS-1);
  wq->w[i] = w;
  barrier();
  wq->head++;
  wq_stat()->push++;
  putwq(wq);
  return 0;
}

int
wq_push1(void (*fn)(struct work *w, void *a0), void *a0)
{
  struct work *w = allocwork();
  if (w == nullptr)
    return -1;
  w->rip = (void*) fn;
  w->arg0 = a0;
  if (wq_push(w) < 0) {
    freework(w);
    return -1;
  }
  return 0;
}

int
wq_push2(void (*fn)(struct work*, void*, void*), void *a0, void *a1)
{
  struct work *w = allocwork();
  if (w == nullptr)
    return -1;
  w->rip = (void*) fn;
  w->arg0 = a0;
  w->arg1 = a1;
  if (wq_push(w) < 0) {
    freework(w);
    return -1;
  }
  return 0;
}

static void
__wq_run(struct work *w)
{
  void (*fn)(struct work*, void*, void*, void*, void*, void*) = 
    (void(*)(work*,void*,void*,void*,void*,void*))w->rip;
  fn(w, w->arg0, w->arg1, w->arg2, w->arg3, w->arg4);
  freework(w);
}

static inline struct work *
__wq_pop(int c)
{
  struct wqueue *wq = &queue[c];
  struct work *w;
  int i;

  i = wq->head;
  if ((i - wq->tail) == 0)
    return 0;
  
  acquire(&wq->lock);
  i = wq->head;
  if ((i - wq->tail) == 0) {
    release(&wq->lock);
    return 0;
  }
  i = (i-1) & (NSLOTS-1);
  w = wq->w[i];
  wq->head--;
  release(&wq->lock);

  wq_stat()->pop++;
  return w;
}

static inline struct work *
__wq_steal(int c)
{
  struct wqueue *wq = &queue[c];
  struct work *w;
  int i;

  if (tryacquire(&wq->lock) == 0)
    return 0;
  i = wq->tail;
  if ((i - wq->head) == 0) {
    release(&wq->lock);
    return 0;
  }
  i = i & (NSLOTS-1);
  w = wq->w[i];
  wq->tail++;
  release(&wq->lock);

  wq_stat()->steal++;
  return w;
}

int
wq_trywork(void)
{
  struct work *w;
  u64 i, k;

  // A "random" victim CPU
  k = rdtsc();

  w = __wq_pop(mycpu()->id);
  if (w != nullptr) {
    __wq_run(w);
    return 1;
  }

  for (i = 0; i < NCPU; i++) {
    u64 j = (i+k) % NCPU;

    if (j == mycpu()->id)
        continue;
    
    w = __wq_steal(j);
    if (w != nullptr) {
      __wq_run(w);
      return 1;
    }
  }

  return 0;
}

void
wq_dump(void)
{
  int i;
  for (i = 0; i < NCPU; i++)
    cprintf("push %lu full %lu pop %lu steal %lu\n",
            stat[i].push, stat[i].full, stat[i].pop, stat[i].steal);
}

static void
__test_stub(struct work *w, void *a0, void *a1)
{
  //long i = (long)a0;
  //cprintf("%u: %lu\n", cpunum(), i);
  volatile atomic<int> *running = (volatile atomic<int>*) a1;
  (*running)--;
}

void
testwq(void)
{
  enum { iters = 10 };
  static volatile atomic<int> running(iters);
  u64 e, s;
  long i;
  
  pushcli();
  if (mycpu()->id == 0) {
    microdelay(1);
    s = rdtsc();
    for (i = 0; i < iters; i++) {
      if (wq_push2(__test_stub, (void*)i, (void*)&running) < 0)
        panic("testwq: oops");
    }
    while (running)
      nop_pause();
    e = rdtsc();
    cprintf("testwq: %lu\n", (e-s)/iters);
    wq_dump();
  } else {
    while (running)
      wq_trywork();
  }
  popcli();
}

static struct work **
do_allocwork(struct work **w, int iters)
{
  int i;

  for (i = 0; i < iters; i++)
    w[i] = allocwork();
  return w;
}

static struct work **
do_freework(struct work **w, int iters)
{
  int i;

  for (i = 0; i < iters; i++)
    freework(w[i]);
  return w;
}

void
benchwq(void)
{
  enum { alloc_iters = 100 };

  static volatile atomic<int> running(alloc_iters);
  static struct work *w[alloc_iters];
  u64 e2, e, s;
  long i;
  
  pushcli();
  if (mycpu()->id == 0) {
    microdelay(1);

    // Warm up
    do_allocwork(w, alloc_iters);
    do_freework(w, alloc_iters);

    s = rdtsc();
    do_allocwork(w, alloc_iters);
    e = rdtsc();
    cprintf("allocwork: %lu\n", (e-s)/alloc_iters);

    s = rdtsc();
    do_freework(w, alloc_iters);
    e = rdtsc();
    cprintf("freework: %lu\n", (e-s)/alloc_iters);

    do_allocwork(w, alloc_iters);
    for (i = 0;i < alloc_iters; i++) {
      w[i]->rip = (void*)__test_stub;
      w[i]->arg1 = (void*)&running;
    }

    s = rdtsc();
    for (i = 0; i < alloc_iters; i++) {
      if (wq_push(w[i]) < 0)
        panic("testwq: oops");
    }
    e = rdtsc();
    while (running)
      nop_pause();
    e2 = rdtsc();

    cprintf("wq_push: %lu\n", (e-s)/alloc_iters);
    cprintf("wq finished: %lu\n", (e2-s)/alloc_iters);

    do_allocwork(w, alloc_iters);
    for (i = 0;i < alloc_iters; i++) {
      w[i]->rip = (void*)__test_stub;
      w[i]->arg1 = (void*)&running;
    }

    s = rdtsc();
    for (i = 0; i < alloc_iters; i++) {
      running = 1;
      if (wq_push(w[i]) < 0)
        panic("testwq: oops");
      while (running)
        nop_pause();
    }
    e = rdtsc();
    cprintf("wq_push one: %lu\n", (e-s)/alloc_iters);

    wq_dump();
  }
  popcli();
}

void
initwq(void)
{
  int i;

  for (i = 0; i < NCPU; i++)
    initlock(&queue[i].lock, "wq lock", LOCKSTAT_WQ);
}
