#if defined(LINUX)
#include "user/wqlinux.hh"
#elif defined(XV6_KERNEL)
#include "wqkernel.hh"
#else
#include "wquser.hh"
#endif
#include "percpu.hh"

#define NSLOTS (1 << WQSHIFT)

class wq {
public:
  wq();
  int push(work *w);
  int trywork();
  void dump();

  static void* operator new(unsigned long);

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
  padded_length* len_;
#endif
};

static wq *wq_;

size_t
wq_size(void)
{
  return sizeof(wq);
}

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
  wqarch_init();
}

void
exitwq(void)
{
  wqarch_exit();
}

//
// wq
//
void*
wq::operator new(unsigned long nbytes)
{
  assert(nbytes == sizeof(wq));
  return allocwq(nbytes);
}

wq::wq(void)
{
  int i;

  for (i = 0; i < NCPU; i++)
    wqlock_init(&q_[i].lock);

#if defined(XV6_USER)
  len_ = allocklen(NCPU*sizeof(padded_length));
#endif
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

inline void
wq::inclen(int c)
{
#if defined(XV6_USER)
  __sync_fetch_and_add(&len_[c].v_, 1);
#endif
}

inline void
wq::declen(int c)
{
#if defined(XV6_USER)
  __sync_fetch_and_sub(&len_[c].v_, 1);
#endif
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
  inclen(mycpuid());
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
  declen(c);
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
  declen(c);
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
