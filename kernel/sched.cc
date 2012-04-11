#include "types.h"
#include "kernel.hh"
#include "mmu.h"
#include "amd64.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.hh"
#include "cpu.hh"
#include "bits.hh"
#include "kmtrace.hh"
#include "vm.hh"
#include "wq.hh"
#include "percpu.hh"
#include "sperf.hh"
#include "major.h"
#include "rnd.hh"

enum { sched_debug = 0 };
enum { steal_nonexec = 1 };

class schedule
{
public:
  schedule();
  void enq(proc* entry);
  proc* deq();
  proc* steal(bool nonexec);
  void dump();

  static void* operator new(unsigned long nbytes, schedule* buf) {
    assert(nbytes == sizeof(schedule));
    return buf;
  }

  sched_stat stats_;
  u64 ncansteal_;
  
private:
  void sanity(void);

  struct spinlock lock_ __mpalign__;
  sched_link head_;
  volatile bool cansteal_ __mpalign__;
};
percpu<schedule> schedule_;

static bool cansteal(proc* p, bool nonexec);

schedule::schedule(void)
{
  initlock(&lock_, "schedule::lock_", LOCKSTAT_SCHED);
  head_.next = &head_;
  head_.prev = &head_;
  ncansteal_ = 0;
  stats_.enqs = 0;
  stats_.deqs = 0;
  stats_.steals = 0;
  stats_.misses = 0;
  stats_.idle = 0;
  stats_.busy = 0;
  stats_.schedstart = 0;
}

void
schedule::enq(proc* p)
{
  sched_link* entry = p;
  // Add to tail
  scoped_acquire x(&lock_);
  entry->next = &head_;
  entry->prev = head_.prev;
  head_.prev->next = entry;
  head_.prev = entry;
  if (cansteal((proc*)entry, true))
    if (ncansteal_++ == 0)
      cansteal_ = true;
  sanity();
  stats_.enqs++;
}

proc*
schedule::deq(void)
{   
  if (head_.next == &head_)
    return nullptr;

  ANON_REGION(__func__, &perfgroup);
  // Remove from head
  scoped_acquire x(&lock_);
  sched_link* entry = head_.next;
  if (entry == &head_)
    return nullptr;
  
  entry->next->prev = entry->prev;
  entry->prev->next = entry->next;
  if (cansteal((proc*)entry, true))
    if (--ncansteal_ == 0)
      cansteal_ = false;
  sanity();
  stats_.deqs++;
  return (proc*)entry;
}

proc*
schedule::steal(bool nonexec)
{
  if (!cansteal_ || !tryacquire(&lock_))
    return nullptr;

  ANON_REGION(__func__, &perfgroup);
  for (sched_link* ptr = head_.next; ptr != &head_; ptr = ptr->next)
    if (cansteal((proc*)ptr, nonexec)) {
      ptr->next->prev = ptr->prev;
      ptr->prev->next = ptr->next;
      if (--ncansteal_ == 0)
        cansteal_ = false;
      sanity();
      ++stats_.steals;
      release(&lock_);
      return (proc*)ptr;
    }
  release(&lock_);
  ++stats_.misses;
  return nullptr;
}

void
schedule::dump(void)
{
  cprintf("%8lu %8lu %8lu %8lu\n",
          stats_.enqs,
          stats_.deqs,
          stats_.steals,
          stats_.misses);
  
  stats_.enqs = 0;
  stats_.deqs = 0;
  stats_.steals = 0;
  stats_.misses = 0;
}

void
schedule::sanity(void)
{
#if DEBUG
  u64 n = 0;

  for (sched_link* ptr = head_.next; ptr != &head_; ptr = ptr->next)
    if (cansteal((proc*)ptr, true))
      n++;
  
  if (n != ncansteal_)
    panic("schedule::sanity: %lu != %lu", n, ncansteal_);
#endif
}

static bool
cansteal(proc* p, bool nonexec)
{
  return (p->get_state() == RUNNABLE && !p->cpu_pin && 
          (p->in_exec_ || nonexec) &&
          p->curcycles != 0 && p->curcycles > VICTIMAGE);
}

void
post_swtch(void)
{
  if (mycpu()->prev->get_state() == RUNNABLE && 
      mycpu()->prev != idleproc())
    addrun(mycpu()->prev);
  release(&mycpu()->prev->lock);
  wqcrit_trywork();
}

int
steal(void)
{
  struct proc *steal;
  int r = 0;
  u64 s = rnd();

  pushcli();

  for (int nonexec = 0; nonexec < (steal_nonexec ? 2 : 1); nonexec++) { 
    for (u64 i = 0; i < NCPU; i++) {
      u64 k = (s+i) % NCPU;
      if (k == myid())
        continue;

      steal = schedule_[k].steal(nonexec);
      if (steal != nullptr) {
        acquire(&steal->lock);
        if (steal->get_state() == RUNNABLE && !steal->cpu_pin &&
            steal->curcycles != 0 && steal->curcycles > VICTIMAGE)
        {
          steal->curcycles = 0;
          steal->cpuid = mycpu()->id;
          addrun(steal);
          release(&steal->lock);
          r = 1;
          goto found;
        }
        if (steal->get_state() == RUNNABLE) {
          addrun(steal);
        }
        release(&steal->lock);
      }
    }
  }

 found:
  popcli();
  return r;
}

void
scheddump(void)
{
  cprintf("\ncpu    enqs     deqs   steals   misses\n");
  for (int i = 0; i < NCPU; i++) {
    cprintf("%-2u ", i);
    schedule_[i].dump();
  }
}

void
addrun(struct proc* p)
{
  ANON_REGION(__func__, &perfgroup);
  p->set_state(RUNNABLE);
  schedule_[p->cpuid].enq(p);
}

void
sched(void)
{
  extern void threadstub(void);
  extern void forkret(void);
  extern void idleheir(void *x);
  int intena;
  proc* prev;
  proc* next;

#if SPINLOCK_DEBUG
  if(!holding(&myproc()->lock))
    panic("sched proc->lock");
#endif
  if (myproc() == idleproc() && 
      myproc()->get_state() != RUNNABLE) {
    extern void idlebequeath(void);
    idlebequeath();
  }

  if(mycpu()->ncli != 1)
    panic("sched locks");
  if(myproc()->get_state() == RUNNING)
    panic("sched running");
  if(readrflags()&FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
  myproc()->curcycles += rdtsc() - myproc()->tsc;

  // Interrupts are disabled
  next = schedule_->deq();

  u64 t = rdtsc();
  if (myproc() == idleproc())
    schedule_->stats_.idle += t - schedule_->stats_.schedstart;
  else
    schedule_->stats_.busy += t - schedule_->stats_.schedstart;
  schedule_->stats_.schedstart = t;
  
  if (next == nullptr) {
    if (myproc()->get_state() != RUNNABLE ||
        // proc changed its CPU pin?
        myproc()->cpuid != mycpu()->id) {
      next = idleproc();
    } else {
      myproc()->set_state(RUNNING);
      mycpu()->intena = intena;
      release(&myproc()->lock);
      return;
    }
  }

  if (next->get_state() != RUNNABLE)
    panic("non-RUNNABLE next %s %u", next->name, next->get_state());

  prev = myproc();
  mycpu()->proc = next;
  mycpu()->prev = prev;

  if (prev->get_state() == ZOMBIE)
    mtstop(prev);
  else
    mtpause(prev);
  mtign();

  switchvm(next);
  next->set_state(RUNNING);
  next->tsc = rdtsc();

  if (next->context->rip != (uptr)forkret && 
      next->context->rip != (uptr)threadstub &&
      next->context->rip != (uptr)idleheir)
  {
    mtresume(next);
  }
  mtrec();

  swtch(&prev->context, next->context);
  mycpu()->intena = intena;
  post_swtch();
}

static int
statread(struct inode *inode, char *dst, u32 off, u32 n)
{
  // Sort of like a binary /proc/stat
  size_t sz = NCPU*sizeof(sched_stat);

  if (n != sz)
    return -1;

  for (int i = 0; i < NCPU; i++) {
    memcpy(&dst[i*sizeof(sched_stat)], &schedule_[i].stats_,
           sizeof(schedule_[i].stats_));
  }
  
  return n;
}

void
initsched(void)
{
  for (int i = 0; i < NCPU; i++)
    new (&schedule_[i]) schedule();

  devsw[MAJ_STAT].write = nullptr;
  devsw[MAJ_STAT].read = statread;
}
