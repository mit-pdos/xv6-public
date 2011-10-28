#include "types.h"
#include "param.h"
#include "kernel.h"
#include "mmu.h"
#include "amd64.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"
#include "cpu.h"
#include "kmtrace.h"

struct rcu {
  unsigned long epoch;
  TAILQ_ENTRY(rcu) link;
  union {
    struct {
      void (*dofree)(void *);
      void *item;
    } f1;

    struct {
      void (*dofree)(int, u64);
      int arg1;
      u64 arg2;
    } f2;
  };
  int type;
};

TAILQ_HEAD(rcu_head, rcu);

static struct { struct rcu_head x __mpalign__; } rcu_q[NCPU];
static u64 global_epoch __mpalign__;
static struct { struct spinlock l __mpalign__; } rcu_lock[NCPU];
static struct { int v __mpalign__; } delayed_nfree[NCPU];
static struct { struct condvar cv __mpalign__; } rcu_cv[NCPU];

enum { rcu_debug = 0 };

struct rcu *
rcu_alloc()
{
  return kmalloc(sizeof(struct rcu));
}

void *
rcu_min(void *vkey, void *v, void *arg){
  u64 *min_epoch_p = arg;
  struct proc *p = (struct proc *) v;
  if (*min_epoch_p > p->epoch) {
      *min_epoch_p = p->epoch;
  }
  return 0;
}

// XXX use atomic instruction to update list (instead of holding lock)
// lists of lists?
void
rcu_gc_work(void)
{
  struct rcu *r, *nr;
  u64 min_epoch = global_epoch;
  int n = 0;

  ns_enumerate(nspid, rcu_min, &min_epoch);

  // pushcli(); // not necessary: rcup->cpu_pin==1
  acquire(&rcu_lock[mycpu()->id].l);

  for (r = TAILQ_FIRST(&rcu_q[mycpu()->id].x); r != NULL; r = nr) {
    if (r->epoch >= min_epoch)
      break;
    release(&rcu_lock[mycpu()->id].l);

    // cprintf("free: %d (%x %x)\n", r->epoch, r->dofree, r->item);
    switch (r->type) {
    case 1:
      r->f1.dofree(r->f1.item);
      break;
    case 2:
      r->f2.dofree(r->f2.arg1, r->f2.arg2);
      break;
    default:
      panic("rcu type");
    }

    acquire(&rcu_lock[mycpu()->id].l);
    delayed_nfree[mycpu()->id].v--;
    n++;
    nr = TAILQ_NEXT(r, link);
    TAILQ_REMOVE(&rcu_q[mycpu()->id].x, r, link);
    kmfree(r);
  }
  release(&rcu_lock[mycpu()->id].l);
  if (rcu_debug)
    cprintf("rcu_gc: cpu %d n %d delayed_nfree=%d min_epoch=%d\n",
	    mycpu()->id, n, delayed_nfree[mycpu()->id], min_epoch);
  // popcli(); // not necessary: rcup->cpu_pin==1

  // global_epoch can be bumped anywhere; this seems as good a place as any
  __sync_fetch_and_add(&global_epoch, 1);
}

void
rcu_gc_worker(void)
{
  release(&myproc()->lock);	// initially held by scheduler

  mtstart(rcu_gc_worker, myproc());

  struct spinlock wl;
  initlock(&wl, "rcu_gc_worker");   // dummy lock

  for (;;) {
    rcu_gc_work();

    acquire(&wl);
    cv_sleep(&rcu_cv[mycpu()->id].cv, &wl);
    release(&wl);
  }
}

void
rcu_gc(void)
{
  cv_wakeup(&rcu_cv[mycpu()->id].cv);
}

// XXX Use atomic instruction to update list (instead of holding lock)
static void
rcu_delayed_int(struct rcu *r)
{
  pushcli();
  acquire(&rcu_lock[mycpu()->id].l);
  // cprintf("rcu_delayed: %d\n", global_epoch);
  TAILQ_INSERT_TAIL(&rcu_q[mycpu()->id].x, r, link);
  delayed_nfree[mycpu()->id].v++;
  release(&rcu_lock[mycpu()->id].l);
  popcli();
}

void
rcu_delayed(void *e, void (*dofree)(void *))
{
  if (rcu_debug)
    cprintf("rcu_delayed: %x %x\n", dofree, e);

  struct rcu *r = rcu_alloc();
  if (r == 0)
    panic("rcu_delayed");
  r->f1.dofree = dofree;
  r->f1.item = e;
  r->epoch = global_epoch;
  r->type = 1;
  rcu_delayed_int(r);
}

void
rcu_delayed2(int a1, u64 a2, void (*dofree)(int,u64))
{
  struct rcu *r = rcu_alloc();
  if (r == 0)
    panic("rcu_delayed");
  r->f2.dofree = dofree;
  r->f2.arg1 = a1;
  r->f2.arg2 = a2;
  r->epoch = global_epoch;
  r->type = 2;
  rcu_delayed_int(r);
}

void
rcu_begin_read(void)
{
  if (myproc() && myproc()->rcu_read_depth++ == 0)
    myproc()->epoch = global_epoch;
  __sync_synchronize();
}

void
rcu_end_read(void)
{
  if (myproc() && myproc()->rcu_read_depth > 0 &&
      --myproc()->rcu_read_depth == 0)
    myproc()->epoch = INF;
}

void
rcu_begin_write(struct spinlock *l)
{
  if (l)
    acquire(l);
  __sync_synchronize();
  rcu_begin_read();
}

void
rcu_end_write(struct spinlock *l)
{
  rcu_end_read();
  if (l)
    release(l);
}

void
initrcu(void)
{
  for (int i = 0; i < NCPU; i++) {
    initlock(&rcu_lock[i].l, "rcu");
    TAILQ_INIT(&rcu_q[i].x);
    initcondvar(&rcu_cv[i].cv, "rcu_gc_cv");
  }
}
