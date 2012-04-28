// Garbage collector for objects with distributed reference counts
//
// Objects maintain per-core reference counts.  When a core's
// reference count drops below 1, the object adds itself to a per-core
// "maybe free" list.  Because multiple cores may drop the reference
// count of a single object below 1, it may appear on multiple "maybe
// free" lists simultaneously.
//
// The garbage collector itself runs periodically and operates in two
// phases.  Phase 1 is reconciliation: each core walks its maybe free
// list and sums up the counters.  If the sum is zero, they race to
// add the object to a per-core "to free" list.  Once phase 1 is
// complete, each object with a zero reference count exists on exactly
// one "to free" list.  In phase 2, all cores free every object on
// their local to free list.
//
// This has the nice property that an object that is only manipulated
// by a single core will remain local to that core.  Also, the cost of
// garbage collection is proportional to the number of objects that
// might be free, rather than the number of objects in the system.

#include "types.h"
#include "kernel.hh"
#include "distref.hh"
#include "wq.hh"
#include "condvar.h"
#include "proc.hh"
#include "kstream.hh"

static console_stream debug(false);

#if DEBUG
constexpr int DISTREF_MAX_PENDING = 2;
#else
constexpr int DISTREF_MAX_PENDING = 100;
#endif

// We use 'null' to indicate that a distributed_refcnt isn't on a free
// list, which means we need something else to terminate a free list.
static constexpr distributed_refcnt *terminator = (distributed_refcnt*)-1;

// A per-core worker that manages a maybe-free list and a to-free
// list.
class distref_worker : public work
{
  distributed_refcnt *maybe_free;
  u64 num_maybe_free;
  distributed_refcnt *to_free;

public:
  distref_worker()
    : maybe_free(nullptr), num_maybe_free(0), to_free(terminator) { }

  // Enqueue a reference counted object to the maybe-free list
  bool enqueue(distributed_refcnt *dr);

  void start(int cpu);

  void run();

private:
  void reconcile();
  void cleanup();
};

static percpu<distref_worker> workers;
static atomic<int> nworkers;
static enum {
  PHASE_RECONCILE,
  PHASE_CLEANUP
} phase;

static spinlock wake_lock("distref_thread lock");
static condvar wake_cv("distref_thread cv");

// Pure virtual destructors need an implementation, even though it
// will never be called automatically (yay C++!)
distributed_refcnt::~distributed_refcnt() { }

bool
distref_worker::enqueue(distributed_refcnt *dr)
{
  if (dr->ref_->next != nullptr)
    return false;
  dr->ref_->next = maybe_free;
  maybe_free = dr;
  return (++num_maybe_free >= DISTREF_MAX_PENDING);
}

void
distref_worker::start(int cpu)
{
  distributed_refcnt *q = (phase == PHASE_RECONCILE ? maybe_free : to_free);
  if (!q)
    nworkers--;
  else if (wqcrit_push(this, cpu) < 0)
    panic("distref_worker::start: failed to push work");
}

void
distref_worker::run()
{
  if (phase == PHASE_RECONCILE)
    reconcile();
  else
    cleanup();

  if (--nworkers == 0) {
    // We're the last worker.  Wake up the main thread.
    acquire(&wake_lock);
    cv_wakeup(&wake_cv);
    release(&wake_lock);
  }
}

void
distref_worker::reconcile()
{
  debug.println("distref: reconciling on ", myid());

  // Unlink the maybe_free list.  This has to be done atomically with
  // respect to distref_check on this CPU.
  pushcli();
  distributed_refcnt *maybe_free = this->maybe_free;
  this->maybe_free = nullptr;
  num_maybe_free = 0;
  popcli();

  // Make a list of objects to free
  distributed_refcnt *to_free = terminator;

  distributed_refcnt *next;
  for (distributed_refcnt *dr = maybe_free; dr; dr = next) {
    struct distref_counter *counter = dr->ref_.get_unchecked();
    next = counter->next;

    // Clear the next pointer so a concurrent distref_check can put it
    // back on the maybe_free list.
    counter->next = nullptr;

    // Sum the counter and the generation counts
    s64 sum = 0;
    u64 gen = 0;
    for (int i = 0; i < NCPU; i++) {
      s64 count_gen = dr->ref_[i].count_gen;
      sum += count_gen >> distref_gen_bits;
      gen += count_gen & (((u64)1 << distref_gen_bits) - 1);
    }

    // Check that we got a stable snapshot
    for (int i = 0; i < NCPU; i++) {
      s64 count_gen = dr->ref_[i].count_gen;
      gen -= count_gen & (((u64)1 << distref_gen_bits) - 1);
    }

    if (gen) {
      // We could retry here, but we don't have to.  If the reference
      // count wasn't stable, then it was non-zero when we started and
      // recent changes will have re-enqueued the object if there's a
      // possibility that it's zero now.
      debug.println("distref: unstable count on ", myid());
      continue;
    }

    if (!sum) {
      // This object needs to be freed.  It could be on multiple
      // maybe-free lists, so check if we're the first to find it.
      if (cmpxch(&dr->next_free_, (decltype(to_free))nullptr, to_free)) {
        debug.println("distref: garbage ", dr, " on ", myid());
        to_free = dr;
      }
    } else {
      debug.println("distref: non-garbage ", dr, " sum ", sum, " on ", myid());
    }
  }

  // Remember our to-free list for phase 2.
  this->to_free = to_free;
}

void
distref_worker::cleanup()
{
  debug.println("distref: cleaning on ", myid());

  // Get our to-free list
  distributed_refcnt *to_free = this->to_free;
  this->to_free = terminator;

  // Free!
  distributed_refcnt *next;
  for (distributed_refcnt *dr = to_free; dr != terminator; dr = next) {
    next = dr->next_free_;
    assert(next != dr);
    dr->distref_free();
  }
}

// Distributed reference count GC control thread
static void
distref_thread(void *x)
{
  for (;;) {
    acquire(&wake_lock);
    cv_sleepto(&wake_cv, &wake_lock, nsectime() + 1000000000);
    release(&wake_lock);

    // Phase 1: Reconcile reference counts
    phase = PHASE_RECONCILE;
    nworkers = NCPU;
    for (int i = 0; i < NCPU; i++)
      workers[i].start(i);

    // Barrier
    acquire(&wake_lock);
    while (nworkers)
      cv_sleep(&wake_cv, &wake_lock);
    release(&wake_lock);

    // Phase 2: Free garbage
    phase = PHASE_CLEANUP;
    nworkers = NCPU;
    for (int i = 0; i < NCPU; i++)
      workers[i].start(i);

    // Barrier
    acquire(&wake_lock);
    while (nworkers)
      cv_sleep(&wake_cv, &wake_lock);
    release(&wake_lock);
  }
}

static void
distref_wakeup()
{
  cv_wakeup(&wake_cv);
}

void
distref_check(distributed_refcnt *dr)
{
  static bool initialized;
  static spinlock initlock("distref initlock");

  if (!initialized) {
    acquire(&initlock);
    if (!initialized) {
      struct proc *t = threadalloc(distref_thread, nullptr);
      acquire(&t->lock);
      safestrcpy(t->name, "distref_thread", sizeof(t->name));
      addrun(t);
      release(&t->lock);
      initialized = true;
    }
    release(&initlock);
  }

  // Add it to the maybe-free list
  if (workers.load()->enqueue(dr))
    distref_wakeup();
}
