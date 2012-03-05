#define WQSHIFT 7
#define NSLOTS (1 << WQSHIFT)
#define CACHELINE 64
#define NCPU 2

#include "include/compiler.h"
#include "include/wq.hh"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "util.h"

typedef uint64_t u64;
typedef uint32_t u32;

static inline u64
rdtsc(void)
{
  u32 hi, lo;
  __asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
  return ((u64)lo)|(((u64)hi)<<32);
}

struct wqueue {
  struct work *w[NSLOTS];
  
  volatile int head __mpalign__;
  volatile int tail;
  pthread_spinlock_t lock;
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
static __thread int myid_;

#define acquire pthread_spin_lock
#define release pthread_spin_unlock
static inline int
tryacquire(pthread_spinlock_t *l)
{
  return (pthread_spin_trylock(l) == 0);
}

static inline int 
myid(void)
{
  return myid_;
}

static inline struct wqstat *
wq_stat(void)
{
  return &stat[myid()];
}

void
freework(struct work *w)
{
  free(w);
}

struct work *
allocwork(void)
{
  return (struct work *)malloc(4096);
}

int
wq_push(struct work *w)
{
  int i;

  struct wqueue *wq = &queue[myid()];
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
  return 0;
}

static void
__wq_run(work *w)
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

  w = __wq_pop(myid());
  if (w != nullptr) {
    __wq_run(w);
    return 1;
  }

  for (i = 0; i < NCPU; i++) {
    u64 j = (i+k) % NCPU;

    if (j == myid())
        continue;
    
    w = __wq_steal(j);
    if (w != nullptr) {
      __wq_run(w);
      return 1;
    }
  }

  return 0;
}

static void
worker_loop(void)
{
  while (1) {
    wq_trywork();
  }
}

static void*
workerth(void *x)
{
  u64 c = (u64)x;
  myid_ = c;
  setaffinity(c);
  worker_loop();
  return NULL;
}

void
initwq(void)
{
  pthread_t th;
  int r;

  myid_ = 0;
  setaffinity(0);

  for (int i = 0; i < NCPU; i++) {
    pthread_spin_init(&queue[i].lock, 0);
  }

  for (int i = 1; i < NCPU; i++) {
    r = pthread_create(&th, NULL, workerth, (void*)(u64)i);
    if (r < 0)
      edie("pthread_create");
  }
}
