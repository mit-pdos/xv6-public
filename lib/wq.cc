#if defined(LINUX)
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

#include "include/types.h"
#include "include/wq.hh"
static __thread int myid_;

int
mycpuid(void)
{
  return myid_;
}

static inline void*
allocwq(void)
{
  return malloc(sizeof(wq));
}

static inline void
wqlock_acquire(wqlock_t *lock)
{
  pthread_spin_lock(lock);
}

static inline int
wqlock_tryacquire(wqlock_t *lock)
{
  return (pthread_spin_trylock(lock) == 0);
}

static inline void
wqlock_release(wqlock_t *lock)
{
  pthread_spin_unlock(lock);
}

static inline void
wqlock_init(wqlock_t *lock) 
{
  pthread_spin_init(lock, 0);
}

static inline u64
rdtsc(void)
{
  u32 hi, lo;
  __asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
  return ((u64)lo)|(((u64)hi)<<32);
}

#define xprintf        printf
#define xmalloc(n)     malloc(n)
#define xfree(p, sz)   free(p)
#define pushcli()
#define popcli()

#elif defined(XV6_KERNEL)

#include "types.h"
#include "kernel.hh"
#include "spinlock.h"
#include "amd64.h"
#include "cpu.hh"
#include "kalloc.hh"
#include "wq.hh"
static inline int
mywqid(void)
{
  return mycpu()->id;
}

static inline void*
allocwq(void)
{
  return ksalloc(slab_wq);
}

static inline void
wqlock_acquire(wqlock_t *lock)
{
  acquire(lock);
}

static inline int
wqlock_tryacquire(wqlock_t *lock)
{
  return tryacquire(lock);
}

static inline void
wqlock_release(wqlock_t *lock)
{
  release(lock);
}

static inline void
wqlock_init(wqlock_t *lock) 
{
  initlock(lock, "wq lock", LOCKSTAT_WQ);
}

#define xprintf      cprintf 
#define xmalloc(n)   kmalloc(n) 
#define xfree(p, sz) kmfree(p, sz)

#else
#warning "Unknown wq implementation"
#endif

static wq *wq_;

int
wq_push(work *w)
{
  return wq_->push(w);
}

int
wq_trywork(void)
{
  return wq_->trywork();
}

void
wq_dump(void)
{
  return wq_->dump();
}

void
initwq(void)
{
  wq_ = new wq();
}

//
// wq
//
void*
wq::operator new(unsigned long nbytes)
{
  assert(nbytes == sizeof(wq));
  return allocwq();
}

wq::wq(void)
{
  int i;

  for (i = 0; i < NCPU; i++)
    wqlock_init(&q_[i].lock);
}

void
wq::dump(void)
{
  int i;
  for (i = 0; i < NCPU; i++)
    xprintf("push %lu full %lu pop %lu steal %lu\n",
            stat_[i].push, stat_[i].full,
            stat_[i].pop, stat_[i].steal);
}

int
wq::push(work *w)
{
  int i;

  pushcli();
  i = q_->head;
  if ((i - q_->tail) == NSLOTS) {
    stat_->full++;
    popcli();
    return -1;
  }
  i = i & (NSLOTS-1);
  q_->w[i] = w;
  barrier();
  q_->head++;
  stat_->push++;
  popcli();
  return 0;
}

inline work*
wq::pop(int c)
{
  struct wqueue *q = &q_[c];
  work *w;
  int i;

  i = q->head;
  if ((i - q->tail) == 0)
    return 0;
  
  wqlock_acquire(&q->lock);
  i = q->head;
  if ((i - q->tail) == 0) {
    wqlock_release(&q->lock);
    return 0;
  }
  i = (i-1) & (NSLOTS-1);
  w = q->w[i];
  q->head--;
  wqlock_release(&q->lock);

  stat_->pop++;
  return w;
}

inline work*
wq::steal(int c)
{
  struct wqueue *q = &q_[c];
  work *w;
  int i;

  if (wqlock_tryacquire(&q->lock) == 0)
    return 0;
  i = q->tail;
  if ((i - q->head) == 0) {
    wqlock_release(&q->lock);
    return 0;
  }
  i = i & (NSLOTS-1);
  w = q->w[i];
  q->tail++;
  wqlock_release(&q->lock);

  stat_->steal++;
  return w;
}

int
wq::trywork(void)
{
  work *w;
  u64 i, k;

  // A "random" victim CPU
  k = rdtsc();

  w = pop(mycpuid());
  if (w != nullptr) {
    w->run();
    return 1;
  }

  for (i = 0; i < NCPU; i++) {
    u64 j = (i+k) % NCPU;

    if (j == mycpuid())
        continue;
    
    w = steal(j);
    if (w != nullptr) {
      w->run();
      return 1;
    }
  }

  return 0;
}

//
// cwork
//
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
  return xmalloc(sizeof(cwork));
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
  xfree(p, sizeof(cwork));
}
