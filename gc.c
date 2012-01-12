#include "types.h"
#include "kernel.h"
#include "mmu.h"
#include "amd64.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"
#include "cpu.h"

// GC scheme based on Fraser's:
// a machine has a global_epoch
// a process maintain an epoch (>= global_epoch)
// one gc thread and state (e.g., NEPOCH delaylists and one tofreelists) per core
// a process add to its core epoch's delayed freelist on delayed_free
// a gcc performs two jobs:
// 1. one gcc thread perform step 1:
//   updates a thread's epoch, when not in an epoch
//   compute min over all process's epochs, and sets global_epoch to min
//   move a core's (global_epoch-2)'s delayed list to a core's tofree list
//   (costs linear in the number of processes.)
// 2. in parallel gc threads free the elements on the tofree list (till global_epoach)
//   (costs linear in the number of elements to be freed)

enum { gc_debug = 0 };

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

static struct gc_state { 
  struct condvar cv;
  struct gc delayed[NEPOCH];
  struct gc tofree[NEPOCH];
  int ndelayed;
  int min_epoch;
} __mpalign__ gc_state[NCPU] __mpalign__;

static struct { struct spinlock l __mpalign__; } gc_lock;
u64 global_epoch __mpalign__;

struct gc *
gc_alloc()
{
  struct gc *r = kmalloc(sizeof(struct gc));
  assert(r);
  gc_state[mycpu()->id].ndelayed++;
  return r;
}

static void *
gc_min(void *vkey, void *v, void *arg){
  u64 *min_epoch_p = arg;
  struct proc *p = (struct proc *) v;
  acquire(&p->gc_epoch_lock);
  if (p->epoch_depth == 0) {
    p->epoch = global_epoch;  
  }
  release(&p->gc_epoch_lock);
  if (*min_epoch_p > p->epoch) {
      *min_epoch_p = p->epoch;
  }
  return NULL;
}

static void
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

static int
gc_free_tofreelist(struct gc **head, u64 epoch)
{
  int nfree = 0;
  struct gc *r, *nr;

  for (r = *head; r != NULL; r = nr) {
    if (r->epoch > epoch) {
      cprintf("gc_free_tofreelist: r->epoch %ld > epoch %ld\n", r->epoch, epoch);
      assert(0);
    }
    nr = r->next;
    gc_free_elem(r);
    nfree++;
  }
  *head = r;
  return nfree;
}


// move to free delayed list to tofreelist so that a process can do its own freeing
void *
gc_move_to_tofree_cpu(int c, u64 epoch)
{
  struct gc *head;
  uint32 fe = (epoch - (NEPOCH-2)) % NEPOCH;
  int cas;
  assert(gc_state[c].delayed[fe].epoch == epoch-(NEPOCH-2));   // XXX race with setting epoch = 0
  // unhook list for fe epoch atomically; this shouldn't fail
  head = gc_state[c].delayed[fe].next;
  cas = __sync_bool_compare_and_swap(&(gc_state[c].delayed[fe].next), head, 0);
  assert(cas);

  // insert list into tofree list so that each core can free in parallel and free its elements
  if(gc_state[c].tofree[fe].epoch != gc_state[c].delayed[fe].epoch) {
    cprintf("%d: tofree epoch %lu delayed epoch %lu\n", c, gc_state[c].tofree[fe].epoch,
	    gc_state[c].delayed[fe].epoch);
    assert(0);
  }
  cas = __sync_bool_compare_and_swap(&(gc_state[c].tofree[fe].next), 0, head);
  assert(cas);

  // move delayed NEPOCH's adhead
  gc_state[c].delayed[fe].epoch += NEPOCH;
  assert(gc_state[c].delayed[fe].next == 0);

  return 0;
}

// only one thread should call this function
static void
gc_move_to_tofree(u64 epoch)
{
  if (gc_debug)
    cprintf("%d: free epoch %ld\n", mycpu()->id, epoch);
  for (int c = 0; c < NCPU; c++) {
    gc_move_to_tofree_cpu(c, epoch);
  }
  int ok  = __sync_bool_compare_and_swap(&global_epoch, epoch, epoch+1);
  assert(ok);
}

// If all threads have seen global_epoch, we can move elements in global_epoch-2 to tofreelist
static void
gc_delayfreelist(void)
{
  int r = tryacquire(&gc_lock.l);
  if (r == 0) return;
  assert(r == 1);

  u64 global = global_epoch;
  u64 min = global;
  // make that global_epoch doesn't run into a core's min_epoch
  for (int c = 0; c < NCPU; c++) { 
    int w = gc_state[c].min_epoch + NEPOCH-1;
    if (w < min) {
      min = w;
    }
  }
  myproc()->epoch_depth++;// ensure ns_enumate's call to gc_begin_epoch doesn't have sideeffects
  ns_enumerate(nspid, gc_min, &min);
  myproc()->epoch_depth--;
  if (min >= global) {
    gc_move_to_tofree(min);
  }
  release(&gc_lock.l);
}

static void
gc_delayed_int(struct gc *r)
{
  pushcli();
  int c = mycpu()->id;
  u64 myepoch = myproc()->epoch;
  u64 minepoch = gc_state[c].delayed[myepoch % NEPOCH].epoch;
  if (gc_debug) 
    cprintf("(%d, %d): gc_delayed: %lu ndelayed %d\n", c, myproc()->pid, global_epoch, gc_state[c].ndelayed);
  if (myepoch != minepoch) {
    cprintf("%d: myepoch %lu minepoch %lu\n", myproc()->pid, myepoch, minepoch);
    panic("gc_delayed_int");
  }
  r->epoch = myepoch;
  do {
    r->next = gc_state[c].delayed[myepoch % NEPOCH].next;
  } while (!__sync_bool_compare_and_swap(&(gc_state[c].delayed[myepoch % NEPOCH].next), r->next, r));
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
gc_start(void)
{
  cv_wakeup(&gc_state[mycpu()->id].cv);
}

void
gc_begin_epoch(void)
{
  if (myproc() == NULL) return;
  acquire(&myproc()->gc_epoch_lock);
  if (myproc()->epoch_depth++ > 0)
    goto done;
  myproc()->epoch = global_epoch;  // not atomic, but it never goes backwards
  // __sync_synchronize();
 done:
  release(&myproc()->gc_epoch_lock);
}

void
gc_end_epoch(void)
{
  if (myproc() == NULL) return;
  acquire(&myproc()->gc_epoch_lock);
  --myproc()->epoch_depth;
  release(&myproc()->gc_epoch_lock);
}

static void
gc_worker(void *x)
{
  struct spinlock wl;

  if (VERBOSE)
    cprintf("gc_worker: %d\n", mycpu()->id);

  initlock(&wl, "rcu_gc_worker dummy");   // dummy lock
  for (;;) {
    u64 i;
    acquire(&wl);
    cv_sleep(&gc_state[mycpu()->id].cv, &wl);  
    release(&wl);

    u64 global = global_epoch;
    for (i = gc_state[mycpu()->id].min_epoch; i < global-2; i++) {
      int nfree = gc_free_tofreelist(&(gc_state[mycpu()->id].tofree[i%NEPOCH].next), i);
      gc_state[mycpu()->id].tofree[i%NEPOCH].epoch += NEPOCH;
      if (gc_debug && nfree > 0) {
	cprintf("%d: epoch %d freed %d\n", mycpu()->id, i, nfree);
      }
    }
    gc_state[mycpu()->id].min_epoch = i;
    gc_delayfreelist();
  }
}

void
initprocgc(struct proc *p)
{
  p->epoch = global_epoch;
  initlock(&p->gc_epoch_lock, "per process gc_lock");
}


void
initgc(void)
{
  initlock(&gc_lock.l, "gc");
  global_epoch = NEPOCH-2;

  for (int i = 0; i < NCPU; i++) {
    for (int j = 0; j < NEPOCH; j++) {
      gc_state[i].delayed[j].epoch = j;
      gc_state[i].tofree[j].epoch = j;
    }
    initcondvar(&gc_state[i].cv, "gc_cv");
  }

  for (u32 c = 0; c < NCPU; c++) {
    struct proc *gcp; 

    gcp = threadalloc(gc_worker, NULL);
    if (gcp == NULL)
      panic("threadalloc: gc_worker");

    snprintf(gcp->name, sizeof(gcp->name), "gc_%u", c);
    gcp->cpuid = c;
    gcp->cpu_pin = 1;
    acquire(&gcp->lock);
    gcp->state = RUNNABLE;
    addrun(gcp);
    release(&gcp->lock);
  }
}
