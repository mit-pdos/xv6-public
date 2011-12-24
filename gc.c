#include "types.h"
#include "kernel.h"
#include "mmu.h"
#include "amd64.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"
#include "cpu.h"
#include "kmtrace.h"

#define NEPOCH 4

static struct { struct spinlock l __mpalign__; } rcu_lock[NCPU];
static struct { struct condvar cv __mpalign__; } rcu_cv[NCPU];

struct gc {
  u64 epoch;
  struct gc *next;  
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
} __mpalign__;
struct gc gc_epoch[NEPOCH][NCPU] __mpalign__;
u64 global_epoch __mpalign__;
int ndelayed __mpalign__;

enum { rcu_debug = 0 };

struct gc *
gc_alloc()
{
  return kmalloc(sizeof(struct gc));
}

void *
gc_min(void *vkey, void *v, void *arg){
  u64 *min_epoch_p = arg;
  struct proc *p = (struct proc *) v;
  if (*min_epoch_p > p->epoch) {
      *min_epoch_p = p->epoch;
  }
  return 0;
}

void
gc_free_elem(struct gc *r)
{
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
  kmfree(r);
}

// Fraser's reclaimation scheme: free all delayed-free items in global_epoch-2
static void
gc_free_epoch(u64 epoch)
{
  cprintf("free epoch %d\n", epoch);

  for (int j = 0; j < NCPU; j++) {
    if (__sync_bool_compare_and_swap(&global_epoch, epoch, epoch+1)) {
      // only one core succeeds; that core in charge of freeing epoch
      struct gc *head;
      struct gc *r, *nr;
      uint32 fe = (epoch - (NEPOCH-2)) % NEPOCH;
      int cas;

      if (gc_epoch[fe][j].epoch != epoch - (NEPOCH-2))
	panic("gc_free_epoch");

      // unhook list for fe epoch atomically
      head = gc_epoch[fe][j].next;
      // this shouldn't fail, because no core is modifying it.
      cas = __sync_bool_compare_and_swap(&gc_epoch[fe][j].next, head, 0);
      if (!cas) panic("gc_free_epoch");
      // free list items on the delayed list
      for (r = head; r != NULL; r = nr) {
	if (r->epoch > epoch-(NEPOCH-2)) {
	  cprintf("%lu %lu\n", r->epoch, epoch-(NEPOCH-2));
	  panic("gc_free_epoch");
	}
	nr = r->next;
	gc_free_elem(r);
	int x = __sync_fetch_and_sub(&ndelayed, 1);
	if (x < 0) panic("gc_free_epoch");
      }
      if (gc_epoch[fe][j].next != 0)
	panic("gc_free_epoch");
      gc_epoch[fe][j].epoch = gc_epoch[fe][j].epoch + NEPOCH;
    }
  }
}

void
gc(void)
{
  u64 global = global_epoch;
  u64 min = global;
  ns_enumerate(nspid, gc_min, &min);
  // cprintf("gc: global %lu min %lu ndelay %d\n", global_epoch, min, ndelayed);
  if (min >= global) {
    gc_free_epoch(min);
  }
}


static void
gc_worker(void *x)
{
  struct spinlock wl;

  initlock(&wl, "rcu_gc_worker");   // dummy lock

  for (;;) {
    gc();

    acquire(&wl);
    cv_sleep(&rcu_cv[mycpu()->id].cv, &wl);
    release(&wl);
  }
}

void
gc_start(void)
{
  cv_wakeup(&rcu_cv[mycpu()->id].cv);
}

static void
gc_delayed_int(struct gc *r)
{
  pushcli();
  u64 myepoch = myproc()->epoch;
  u64 minepoch = gc_epoch[myepoch % NEPOCH][mycpu()->id].epoch;
  // cprintf("%d: gc_delayed: %lu ndelayed %d\n", myproc()->pid, global_epoch, ndelayed);
  if (myepoch != minepoch) {
    cprintf("%d: myepoch %lu minepoch %lu\n", myproc()->pid, myepoch, minepoch);
    panic("gc_delayed_int");
  }
  r->epoch = myepoch;
  do {
    r->next = gc_epoch[myepoch % NEPOCH][mycpu()->id].next;
  } while (!__sync_bool_compare_and_swap(&(gc_epoch[myepoch % NEPOCH][mycpu()->id].next), r->next, r));
   popcli();
}

void
gc_delayed(void *e, void (*dofree)(void *))
{
  struct gc *r = gc_alloc();
  if (r == 0)
    panic("gc_delayed");
  r->f1.dofree = dofree;
  r->f1.item = e;
  r->type = 1;
  gc_delayed_int(r);
}

void
gc_delayed2(int a1, u64 a2, void (*dofree)(int,u64))
{
  struct gc *r = gc_alloc();
  if (r == 0)
    panic("gc_delayed2");
  r->f2.dofree = dofree;
  r->f2.arg1 = a1;
  r->f2.arg2 = a2;
  r->type = 2;
  gc_delayed_int(r);
}

void
gc_begin_epoch(void)
{
  if (myproc() && myproc()->rcu_read_depth++ == 0)
    myproc()->epoch = global_epoch;
  __sync_synchronize();
}

void
gc_end_epoch(void)
{
  if (myproc() && myproc()->rcu_read_depth > 0)
    myproc()->rcu_read_depth--;
}

void
initgc(void)
{
  for (int i = 0; i < NCPU; i++) {
    initlock(&rcu_lock[i].l, "rcu");
    initcondvar(&rcu_cv[i].cv, "rcu_gc_cv");
  }
  global_epoch = NEPOCH-2;
  for (int i = 0; i < NEPOCH; i++) 
    for (int j = 0; j < NEPOCH; j++)
      gc_epoch[i][j].epoch = i;

  for (u32 c = 0; c < NCPU; c++) {
    struct proc *gcp; 

    gcp = threadalloc(gc_worker, NULL);
    if (gcp == NULL)
      panic("threadalloc: gc_worker");

    gcp->cpuid = c;
    gcp->cpu_pin = 1;
    acquire(&gcp->lock);
    gcp->state = RUNNABLE;
    addrun(gcp);
    release(&gcp->lock);
  }
}
