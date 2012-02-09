#if WQENABLE
#include "types.h"
#include "kernel.h"
#include "spinlock.h"
#include "amd64.h"
#include "cpu.h"
#include "wq.h"

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
  return &queue[cpunum()];
}

static inline void
putwq(struct wqueue *wq)
{
  popcli();
}

static inline struct wqstat *
wq_stat(void)
{
  return &stat[cpunum()];
}

static struct work *
allocwork(void)
{
  return (struct work *)kalloc();
}

static void
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
  if (w == NULL)
    return -1;
  w->rip = fn;
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
  if (w == NULL)
    return -1;
  w->rip = fn;
  w->arg0 = a0;
  w->arg1 = a1;
  if (wq_push(w) < 0) {
    freework(w);
    return -1;
  }
  return 0;
}

static struct work *
__wq_pop(int c)
{
  // Called with cli
  struct wqueue *wq = &queue[c];
  struct work *w;
  int i;
  
  acquire(&wq->lock);
  i = wq->head;
  if ((i - wq->tail) == 0) {
    release(&wq->lock);
    return NULL;
  }
  i = (i-1) & (NSLOTS-1);
  w = wq->w[i];
  wq->head--;
  release(&wq->lock);

  wq_stat()->pop++;
  return w;
}

static struct work *
__wq_steal(int c)
{
  // Called with cli
  struct wqueue *wq = &queue[c];
  struct work *w;
  int i;

  acquire(&wq->lock);
  i = wq->tail;
  if ((i - wq->head) == 0) {
    release(&wq->lock);
    return NULL;
  }
  i = i & (NSLOTS-1);
  w = wq->w[i];
  wq->tail++;
  release(&wq->lock);

  wq_stat()->steal++;
  return w;
}

static void
__wq_run(struct work *w)
{
  void (*fn)(struct work*, void*, void*) = w->rip;
  fn(w, w->arg0, w->arg1);
  freework(w);
}

int
wq_trywork(void)
{
  struct work *w;
  int i;

  pushcli();
  w = __wq_pop(mycpu()->id);
  if (w != NULL) {
    __wq_run(w);
    popcli();
    return 1;
  }
  // XXX(sbw) should be random
  for (i = 0; i < NCPU; i++) {
    if (i == mycpu()->id)
        continue;
    
    w = __wq_steal(i);
    if (w != NULL) {
      __wq_run(w);
      popcli();
      return 1;
    }
  }

  popcli();
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
  volatile int *running = a1;
  subfetch(running, 1);
}

void
testwq(void)
{
  enum { iters = 10 };
  static volatile int running = iters;
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
    e = rdtsc();
    cprintf("testwq: %lu\n", (e-s)/iters);
    while (running)
      nop_pause();
    wq_dump();
  } else {
    while (running)
      wq_trywork();
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

#endif // WQENABLE
