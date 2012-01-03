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

#define NGC 1

// 1 worker in total. more workers makes sense, if we have per-core process
// lists.
#define NWORKER 1
static struct { struct condvar cv __mpalign__; } rcu_cv[NWORKER];
static struct { struct spinlock l __mpalign__; } gc_lock;

enum { gc_debug = 0 };

struct gc {
  u64 epoch;
  struct gc *next;  
  struct gc *free;
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

u64 global_epoch __mpalign__;

enum { rcu_debug = 0 };

struct gc *
gc_alloc()
{
  struct gc *r = kmalloc(sizeof(struct gc));
  assert(r);
  myproc()->ndelayed++;
  return r;
}

static void *
gc_min(void *vkey, void *v, void *arg){
  u64 *min_epoch_p = arg;
  struct proc *p = (struct proc *) v;
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
gc_free_list(struct gc *head, u64 epoch)
{
  int nfree = 0;
  struct gc *r, *nr;

  for (r = head; r != NULL; r = nr) {
    if (r->epoch > epoch) {
      cprintf("%lu %lu\n", r->epoch, epoch);
      assert(0);
    }
    nr = r->next;
    gc_free_elem(r);
    nfree++;
  }
  return nfree;
}

// move to free delayed list to free list so that a process can do its own freeing
void *
gc_move_to_free_proc(void *vkey, void *v, void *arg){
  u64 *epoch = arg;
  struct proc *p = (struct proc *) v;
  struct gc *head;
  uint32 fe = (*epoch - (NEPOCH-2)) % NEPOCH;
  int cas;
  assert(p->gc_epoch[fe].epoch == *epoch-(NEPOCH-2));   // XXX race with setting epoch = 0
  // unhook list for fe epoch atomically
  head = p->gc_epoch[fe].next;
  // this shouldn't fail, because no core is modifying it.
  cas = __sync_bool_compare_and_swap(&(p->gc_epoch[fe].next), head, 0);
  assert(cas);
  // insert list into local free list so that each core can do its own frees
  assert (p->gc_epoch[fe].free == 0);
  cas = __sync_bool_compare_and_swap(&(p->gc_epoch[fe].free), 0, head);
  assert(cas);
  assert(p->gc_epoch[fe].next == 0);
  return 0;
}

// Fraser's reclaimation scheme: free all delayed-free items in global_epoch-2
// only one thread should call this function
static void
gc_move_to_free(u64 epoch)
{
  if (gc_debug)
    cprintf("%d: free epoch %ld\n", myproc()->pid, epoch);
  myproc()->rcu_read_depth++;  // ensure ns_enumate's call to gc_begin_epoch doesn't call gc()
  ns_enumerate(nspid, gc_move_to_free_proc, &epoch);
  myproc()->rcu_read_depth--;
  int ok  = __sync_bool_compare_and_swap(&global_epoch, epoch, epoch+1);
  assert(ok);
}

// If all threads have seen global_epoch, we can free elements in global_epoch-2
static void
gc(void)
{
  int r = tryacquire(&gc_lock.l);
  if (r == 0) return;
  assert(r == 1);

  u64 global = global_epoch;
  u64 min = global;
  myproc()->rcu_read_depth++;  // ensure ns_enumate's call to gc_begin_epoch doesn't call gc()
  ns_enumerate(nspid, gc_min, &min);
  myproc()->rcu_read_depth--;
  if (min >= global) {
    gc_move_to_free(min);
  }
  release(&gc_lock.l);
}

static void
gc_delayed_int(struct gc *r)
{
  pushcli();
  u64 myepoch = myproc()->epoch;
  u64 minepoch = myproc()->gc_epoch[myepoch % NEPOCH].epoch;
  if (gc_debug) 
    cprintf("%d: gc_delayed: %lu ndelayed %d\n", myproc()->pid, global_epoch, myproc()->ndelayed);
  if (myepoch != minepoch) {
    cprintf("%d: myepoch %lu minepoch %lu\n", myproc()->pid, myepoch, minepoch);
    panic("gc_delayed_int");
  }
  r->epoch = myepoch;
  do {
    r->next = myproc()->gc_epoch[myepoch % NEPOCH].next;
  } while (!__sync_bool_compare_and_swap(&(myproc()->gc_epoch[myepoch % NEPOCH].next), r->next, r));
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

static void*
gc_free(void *vkey, void *v, void *arg)
{
  struct proc *p = (struct proc *) v;
  acquire(&p->gc_lock);
  u64 global = global_epoch;
  for (u64 epoch = p->epoch; epoch < global; epoch++) {
    int j = (epoch - (NEPOCH - 2)) % NEPOCH;
    assert(p->gc_epoch[j].epoch == epoch-2);
    struct gc *free = p->gc_epoch[j].free;
    int ok = __sync_bool_compare_and_swap(&(p->gc_epoch[j].free), free, NULL);
    assert(ok);
    int nfree = gc_free_list(free, epoch - 2);
    p->ndelayed -= nfree;
    if (gc_debug && nfree > 0)
      cprintf("%d: epoch %d freed %d\n", p->pid, epoch - 2, nfree);
    p->gc_epoch[j].epoch = p->gc_epoch[j].epoch + NEPOCH;
  }
  p->epoch = global;  // not atomic, but it never goes backwards
  __sync_synchronize();
  release(&p->gc_lock);
  return NULL;
}

void
gc_start(void)
{
  cv_wakeup(&rcu_cv[0].cv);   // NWORKER = 1
  //  cv_wakeup(&rcu_cv[mycpu()->id].cv);
}

void
gc_begin_epoch(void)
{
  if (myproc() == NULL) return;
  if (myproc()->rcu_read_depth++ > 0)
    return;
  gc_free(NULL, (void *) myproc(), NULL);
}

void
gc_end_epoch(void)
{
  if (myproc() == NULL) return;
  if (--myproc()->rcu_read_depth > 0)
    return;

#if 0
  // kick gcc early if under memory pressure
  int free = 0;
  for (int j = 0; j < NEPOCH; j++) {
    if (myproc()->gc_epoch[j].free)
      free = 1;
  }
  u64 nd = myproc()->ndelayed;
  if (!free && nd > NGC) {
    gc_start();
  }
#endif
}

static void
gc_worker(void *x)
{
  struct spinlock wl;

  initlock(&wl, "rcu_gc_worker dummy");   // dummy lock
  for (;;) {
    acquire(&wl);

    myproc()->rcu_read_depth++;  // call gc_free once for gc_worker
    ns_enumerate(nspid, gc_free, NULL);
    myproc()->rcu_read_depth--;

    gc();

    cv_sleep(&rcu_cv[0].cv, &wl);   // NWORKER = 1
    release(&wl);
  }
}

void
initprocgc(struct proc *p)
{
  p->epoch = global_epoch;
  p->gc_epoch = kmalloc(sizeof(struct gc) * NEPOCH);
  initlock(&p->gc_lock, "per process gc_lock");
  for (u64 i = global_epoch-2; i < global_epoch+2; i++)  {
    p->gc_epoch[i % NEPOCH].epoch = i;
    p->gc_epoch[i % NEPOCH].free = NULL;
    p->gc_epoch[i % NEPOCH].next = NULL;
  }
}


void
initgc(void)
{
  initlock(&gc_lock.l, "gc");
  global_epoch = NEPOCH-2;

  for (int i = 0; i < NWORKER; i++) {
    initcondvar(&rcu_cv[i].cv, "rcu_gc_cv");
  }

  // one worker for now
  for (u32 c = 0; c < NWORKER; c++) {
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
