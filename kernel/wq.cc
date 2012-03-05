#include "types.h"
#include "kernel.hh"
#include "spinlock.h"
#include "amd64.h"
#include "cpu.hh"
#include "wq.hh"

#define NSLOTS (1 << WQSHIFT)

struct wqueue {
  work *w[NSLOTS];
  
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

int
wq_push(work *w)
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

static inline work*
__wq_pop(int c)
{
  struct wqueue *wq = &queue[c];
  work *w;
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

static inline work*
__wq_steal(int c)
{
  struct wqueue *wq = &queue[c];
  work *w;
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
  work *w;
  u64 i, k;

  // A "random" victim CPU
  k = rdtsc();

  w = __wq_pop(mycpu()->id);
  if (w != nullptr) {
    w->run();
    return 1;
  }

  for (i = 0; i < NCPU; i++) {
    u64 j = (i+k) % NCPU;

    if (j == mycpu()->id)
        continue;
    
    w = __wq_steal(j);
    if (w != nullptr) {
      w->run();
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

void
initwq(void)
{
  int i;

  for (i = 0; i < NCPU; i++)
    initlock(&queue[i].lock, "wq lock", LOCKSTAT_WQ);
}

void
cwork::run(void)
{
  void (*fn)(void*, void*, void*, void*, void*) = 
    (void(*)(void*,void*,void*,void*,void*))rip;
  fn(arg0, arg1, arg2, arg3, arg4);
  delete this;
}

void*
cwork::operator new(unsigned long nbytes)
{
  assert(nbytes == sizeof(cwork));
  return kmalloc(sizeof(cwork));
}

void*
cwork::operator new(unsigned long nbytes, cwork* buf)
{
  assert(nbytes == sizeof(cwork));
  return buf;
}

void
cwork::operator delete(void *p)
{
  kmfree(p, sizeof(cwork));
}

