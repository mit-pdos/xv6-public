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

#define NDELAY 500

struct rcu {
  void *item;
  unsigned long epoch;
  struct rcu *rcu;
  void (*dofree)(void *);
};
static struct rcu *rcu_delayed_head;
static struct rcu *rcu_delayed_tail;
static struct rcu *rcu_freelist;
static uint global_epoch;
static uint min_epoch;
static struct spinlock rcu_lock;

static int delayed_nfree;
static int ninuse;

void
rcuinit(void)
{
  struct rcu *r;
  int i;

  initlock(&rcu_lock, "rcu");
  for (i = 0; i < NRCU; i++) {
    r = (struct rcu *) kmalloc(sizeof(struct rcu));
    r->rcu = rcu_freelist;
    rcu_freelist = r;
  }
  cprintf("rcu_init: allocated %ld bytes\n", sizeof(struct rcu) * NRCU);
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

  ns_enumerate(nspid, rcu_min);
  for (r = rcu_delayed_head; r != NULL; r = nr) {
    if (r->epoch >= min_epoch)
      break;

    // printf("free: %ld\n", r->epoch);
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
  //  printf("rcu_gc: n=%d ndelayed_free=%d nfree=%d ninuse=%d\n", n, delayed_nfree, 
  //	 tree_nfree, tree_ninuse);
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
  //  printf("rcu_delayed: %ld\n", global_epoch);
  if (rcu_delayed_tail != 0) 
    rcu_delayed_tail->rcu = r;
  rcu_delayed_tail = r;
  if (rcu_delayed_head == 0) rcu_delayed_head = r;
  delayed_nfree++;
  ninuse--;
}

void
rcu_begin_read(int tid)
{
  proc->epoch = global_epoch;
  __sync_synchronize();
}

void
rcu_end_read(int tid)
{
  proc->epoch = INF;
}

void
rcu_begin_write(int tid)
{
  acquire(&rcu_lock);
}

void
rcu_end_write(int tid)
{
  // for other data structures using rcu, use atomic add:
  __sync_fetch_and_add(&global_epoch, 1);
  rcu_gc();
  release(&rcu_lock);
}

