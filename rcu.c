#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "x86.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"

#define NRCU 1000

struct rcu {
  void *item;
  unsigned long epoch;
  struct rcu *rcu;
  void (*dofree)(void *);
};
static struct rcu *rcu_delayed_head __attribute__ ((aligned (CACHELINE)));
static struct rcu *rcu_delayed_tail __attribute__ ((aligned (CACHELINE)));
static struct rcu *rcu_freelist __attribute__ ((aligned (CACHELINE)));
static uint global_epoch __attribute__ ((aligned (CACHELINE))); static
uint min_epoch __attribute__ ((aligned (CACHELINE))); static struct
spinlock rcu_lock __attribute__ ((aligned (CACHELINE))); static int
delayed_nfree __attribute__ ((aligned (CACHELINE)));

void
rcuinit(void)
{
  struct rcu *r;
  int i;

  initlock(&rcu_lock, "rcu");
  for (i = 0; i < NRCU; i++) {
    r = (struct rcu *) kmalloc(sizeof(struct rcu));
    memset(r, 0, sizeof(struct rcu));
    r->rcu = rcu_freelist;
    rcu_freelist = r;
  }
  // cprintf("rcu_init: allocated %d bytes\n", sizeof(struct rcu) * NRCU);
}

struct rcu *
rcu_alloc()
{
  struct rcu *r = rcu_freelist;
  if (r == 0) {
    panic("rcu_alloc");
  }
  rcu_freelist = r->rcu;
  return r;
}

void
rcu_min(int key, void *v){
  struct proc *p = (struct proc *) v;
  if (min_epoch > p->epoch) {
      min_epoch = p->epoch;
  }
}

// XXX use atomic instruction to update list (instead of holding lock)
// lists of lists?
void
rcu_gc(void)
{
  struct rcu *r, *nr;
  int n = 0;
  min_epoch = global_epoch;

  acquire(&rcu_lock);

  ns_enumerate(nspid, rcu_min);
  for (r = rcu_delayed_head; r != NULL; r = nr) {
    if (r->epoch >= min_epoch)
      break;
    // cprintf("free: %d\n", r->epoch);
    if (r->dofree == 0)
      panic("rcu_gc");
    r->dofree(r->item);
    delayed_nfree--;
    n++;
    rcu_delayed_head = r->rcu;
    if (rcu_delayed_head == 0)
      rcu_delayed_tail = 0;
    nr = r->rcu;
    r->rcu = rcu_freelist;
    rcu_freelist = r;
  }
  release(&rcu_lock);
  // cprintf("rcu_gc: n %d ndelayed_free=%d\n", n, delayed_nfree);
}

// XXX Use atomic instruction to update list (instead of holding lock)
void
rcu_delayed(void *e, void (*dofree)(void *))
{
  struct rcu *r = rcu_alloc();
  if (r == 0)
    panic("rcu_delayed");
  r->dofree = dofree;
  r->item = e;
  r->rcu = 0;
  r->epoch = global_epoch;
  acquire(&rcu_lock);
  // cprintf("rcu_delayed: %d\n", global_epoch);
  if (rcu_delayed_tail != 0) 
    rcu_delayed_tail->rcu = r;
  rcu_delayed_tail = r;
  if (rcu_delayed_head == 0) rcu_delayed_head = r;
  release(&rcu_lock);
  delayed_nfree++;
}

void
rcu_begin_read(void)
{
  proc->epoch = global_epoch;
  __sync_synchronize();
}

void
rcu_end_read(void)
{
  proc->epoch = INF;
}

void
rcu_begin_write(struct spinlock *l)
{
  acquire(l);
}

// XXX if a process never calls rcu_end_write() we have a problem; run
// rcu_gc from a kernel thread periodically?
void
rcu_end_write(struct spinlock *l)
{
  // for other data structures using rcu, use atomic add:
  __sync_fetch_and_add(&global_epoch, 1);
  release(l);
  rcu_gc();
}

