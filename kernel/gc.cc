#include "crange_arch.hh"
#include "gc.hh"
#include "atomic_util.hh"
#include "ns.hh"

using std::atomic;

extern u64 proc_hash(const u32&);
extern xns<u32, proc*, proc_hash> *xnspid;

// GC scheme based on Fraser's:
// a machine has a global_epoch
// a process maintain an epoch (>= global_epoch)
// one gc thread and state (e.g., NEPOCH delaylists and one tofreelists) per core
// a process add to its core epoch's delayed freelist on delayed_free
// a gcc thread performs two jobs:
// 1. one gcc thread perform step 1:
//   updates a thread's epoch, when not in an epoch
//   compute min over all process's epochs, and sets global_epoch to min
//   move a core's (global_epoch-2)'s delayed list to a core's tofree list
//   (costs linear in the number of processes.)
// 2. in parallel gc threads free the elements on the tofree list (till global_epoach)
//   (costs linear in the number of elements to be freed)

enum { gc_debug = 0 };

#define NGC 10000

struct headinfo {
  atomic<rcu_freed*> head;
  atomic<u64> epoch;
};

static struct gc_state { 
  struct condvar cv;
  headinfo delayed[NEPOCH];
  headinfo tofree[NEPOCH];
  atomic<int> ndelayed;
  int min_epoch;
  int nrun;
  int nfree;
  int cnt;
} __mpalign__ gc_state[NCPU] __mpalign__;

static struct { struct spinlock l __mpalign__; } gc_lock;
atomic<u64> global_epoch __mpalign__;

static int
gc_free_tofreelist(atomic<rcu_freed*> *head, u64 epoch)
{
  int nfree = 0;
  rcu_freed *r, *nr;

  r = *head;
  while (!std::atomic_compare_exchange_strong(head, &r, (rcu_freed*) 0))
    ; /* spin */

  for (; r; r = nr) {
    if (r->_rcu_epoch > epoch) {
      cprintf("gc_free_tofreelist: r->epoch %ld > epoch %ld\n", r->_rcu_epoch, epoch);
#if RCU_TYPE_DEBUG
      cprintf("gc_free_tofreelist: name %s\n", r->_rcu_type);
#endif
      assert(0);
    }
    nr = r->_rcu_next;
    r->do_gc();
    nfree++;
  }
  return nfree;
}


// move to free delayed list to tofreelist so that a process can do its own freeing
void *
gc_move_to_tofree_cpu(int c, u64 epoch)
{
  rcu_freed *head;
  u32 fe = (epoch - (NEPOCH-2)) % NEPOCH;
  assert(gc_state[c].delayed[fe].epoch == epoch-(NEPOCH-2));   // XXX race with setting epoch = 0
  // unhook list for fe epoch atomically; this shouldn't fail
  head = gc_state[c].delayed[fe].head;
  while (!std::atomic_compare_exchange_strong(&gc_state[c].delayed[fe].head,
                                              &head, (rcu_freed*) 0)) {}

  // insert list into tofree list so that each core can free in parallel and free its elements
  if(gc_state[c].tofree[fe].epoch != gc_state[c].delayed[fe].epoch) {
    cprintf("%d: tofree epoch %lu delayed epoch %lu\n", c,
            gc_state[c].tofree[fe].epoch.load(),
	    gc_state[c].delayed[fe].epoch.load());
    assert(0);
  }
  assert(cmpxch(&gc_state[c].tofree[fe].head, (rcu_freed*) 0, head));

  // move delayed NEPOCH's adhead
  gc_state[c].delayed[fe].epoch += NEPOCH;
  assert(gc_state[c].delayed[fe].head == 0);
  // XXX race with gc_delayed()?

  return 0;
}

// only one thread should call this function
static void
gc_move_to_tofree(u64 epoch)
{
  if (gc_debug)
    cprintf("%d: move epoch %ld to tofreelist\n", mycpu()->id, epoch);
  for (int c = 0; c < ncpu; c++) {
    gc_move_to_tofree_cpu(c, epoch);
  }
  assert(cmpxch(&global_epoch, epoch, epoch+1));
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
  for (int c = 0; c < ncpu; c++) { 
    u64 w = gc_state[c].min_epoch + NEPOCH-1;
    if (w < min) {
      min = w;
    }
  }
  if (gc_debug) {
    cprintf("(%d,%d) (%s): min %lu global %lu\n", myproc()->cpuid, myproc()->pid, myproc()->name, min, global);
  }
  myproc()->epoch++; // ensure enumerate's call to gc_begin_epoch doesn't have sideeffects
  xnspid->enumerate([&min](u32, proc *p)->bool{
      // Some threads may never call begin/end_epoch(), and never update
      // p->epoch, so gc_thread does it for them.
      u64 x = p->epoch.load();
      if (!(x & 0xff)) {
        cmpxch(&p->epoch, x, global_epoch.load() << 8);
        x = p->epoch.load();
      }

      // cprintf("gc_min %d(%s): %lu %ld\n", p->pid, p->name, p->epoch, p->epoch_depth);
      if (min > (x>>8))
        min = (x>>8);
      return false;
    });
  myproc()->epoch--;
  if (min >= global) {
    gc_move_to_tofree(min);
  }
  release(&gc_lock.l);
}

void
gc_delayed(rcu_freed *e)
{
  int c = mycpu()->id;
  gc_state[c].ndelayed++;

  u64 myepoch = (myproc()->epoch >> 8);
  u64 minepoch = gc_state[c].delayed[myepoch % NEPOCH].epoch;
  if (gc_debug) 
    cprintf("(%d, %d): gc_delayed: %lu ndelayed %d\n", c, myproc()->pid,
            global_epoch.load(), gc_state[c].ndelayed.load());
  if (myepoch != minepoch) {
    cprintf("%d: myepoch %lu minepoch %lu\n", myproc()->pid, myepoch, minepoch);
    panic("gc_delayed_int");
  }
  e->_rcu_epoch = myepoch;
  e->_rcu_next = gc_state[c].delayed[myepoch % NEPOCH].head;
  while (!cmpxch_update(&gc_state[c].delayed[myepoch % NEPOCH].head, &e->_rcu_next, e)) {}
}

void
gc_begin_epoch(void)
{
  if (myproc() == nullptr) return;
  u64 v = myproc()->epoch++;
  if (v & 0xff)
    return;

  cmpxch(&myproc()->epoch, v+1, (global_epoch.load()<<8)+1);
  // __sync_synchronize();
}

void
gc_end_epoch(void)
{
  if (myproc() == nullptr) return;
  u64 e = --myproc()->epoch;
  if ((e & 0xff) == 0 && gc_state[mycpu()->id].ndelayed > NGC) 
    cv_wakeup(&gc_state[mycpu()->id].cv);
}

void gc_dumpstat(void)
{
  for (int i = 0; i < ncpu; i++) {
    cprintf("worker %d: %d %d\n", i, gc_state[i].nrun, gc_state[i].nfree);
  }
}


static void
gc_worker(void *x)
{
  struct spinlock wl;

  if (VERBOSE)
    cprintf("gc_worker: %d\n", mycpu()->id);

  initlock(&wl, "rcu_gc_worker dummy", LOCKSTAT_GC);   // dummy lock
  for (;;) {
    u64 i;
    acquire(&wl);
    cv_sleepto(&gc_state[mycpu()->id].cv, &wl, nsectime() + 1000000000);
    release(&wl);
    gc_state[mycpu()->id].nrun++;
    u64 global = global_epoch;
    myproc()->epoch = global_epoch.load() << 8;      // move the gc thread to next epoch
    for (i = gc_state[mycpu()->id].min_epoch; i < global-2; i++) {
      int nfree = gc_free_tofreelist(&gc_state[mycpu()->id].tofree[i%NEPOCH].head, i);
      gc_state[mycpu()->id].tofree[i%NEPOCH].epoch += NEPOCH;
      gc_state[mycpu()->id].ndelayed -= nfree;
      if (0 && nfree > 0) {
	cprintf("%d: epoch %lu freed %d\n", mycpu()->id, i, nfree);
      }
      gc_state[mycpu()->id].nfree += nfree;
    }
    gc_state[mycpu()->id].min_epoch = i;
    gc_delayfreelist();
  }
}

void
initprocgc(struct proc *p)
{
  p->epoch = global_epoch.load() << 8;
}

void
initgc(void)
{
  initlock(&gc_lock.l, "gc", LOCKSTAT_GC);
  global_epoch = NEPOCH-2;

  for (int i = 0; i < ncpu; i++) {
    for (int j = 0; j < NEPOCH; j++) {
      gc_state[i].delayed[j].epoch = j;
      gc_state[i].tofree[j].epoch = j;
    }
    initcondvar(&gc_state[i].cv, "gc_cv");
  }

  for (int c = 0; c < ncpu; c++) {
    char namebuf[32];
    snprintf(namebuf, sizeof(namebuf), "gc_%u", c);
    threadpin(gc_worker, 0, namebuf, c);
  }
}
