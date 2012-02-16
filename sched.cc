#include "types.h"
#include "kernel.hh"
#include "mmu.h"
#include "amd64.h"
#include "spinlock.h"
#include "condvar.hh"
#include "queue.h"
#include "proc.hh"
#include "cpu.hh"
#include "bits.hh"
#include "kmtrace.hh"
#include "sched.h"
#include "vm.hh"
#include <stddef.h>

enum { sched_debug = 0 };

struct runq {
  STAILQ_HEAD(queue, proc) q;
  struct spinlock lock;
  __padout__;
};

static struct runq runq[NCPU] __mpalign__;

void
addrun(struct proc *p)
{
  // Always called with p->lock held
  struct runq *q;

  p->state = RUNNABLE;

  q = &runq[p->cpuid];
  acquire(&q->lock);
  STAILQ_INSERT_HEAD(&q->q, p, runqlink);
  p->runq = q;
  release(&q->lock);
}

void
delrun(struct proc *p)
{
  // Always called with p->lock held
  struct runq *q;

  q = p->runq;
  acquire(&q->lock);
  STAILQ_REMOVE(&q->q, p, proc, runqlink);
  release(&q->lock);
}

int
steal(void)
{
  struct proc *steal;
  int i;
  int r = 0;

  pushcli();
  
  for (i = 1; i < NCPU; i++) {
    struct runq *q = &runq[(i+cpunum()) % NCPU];
    struct proc *p;

    // XXX(sbw) Look for a process to steal.  Acquiring q->lock
    // then p->lock can result in deadlock.  So we acquire
    // q->lock, scan for a process, drop q->lock, acquire p->lock,
    // and then check that it's still ok to steal p.
    steal = NULL;
    if (tryacquire(&q->lock) == 0)
      continue;
    STAILQ_FOREACH(p, &q->q, runqlink) {
      if (p->state == RUNNABLE && !p->cpu_pin && 
          p->curcycles != 0 && p->curcycles > VICTIMAGE)
      {
        steal = p;
        break;
      }
    }
    release(&q->lock);

    if (steal) {
      acquire(&steal->lock);
      if (steal->state == RUNNABLE && !steal->cpu_pin &&
          steal->curcycles != 0 && steal->curcycles > VICTIMAGE)
      {
        delrun(steal);
        steal->curcycles = 0;
        steal->cpuid = mycpu()->id;
        addrun(steal);
        release(&steal->lock);
        r = 1;
        break;
      }
      release(&steal->lock);
    }
  }

  popcli();
  return r;
}

struct proc *
schednext(void)
{
  // No locks, interrupts enabled
  struct runq *q;
  struct proc *p = NULL;

  pushcli();
  q = &runq[cpunum()];
  acquire(&q->lock);
  p = STAILQ_LAST(&q->q, proc, runqlink);
  if (p) {
    STAILQ_REMOVE(&q->q, p, proc, runqlink);
    STAILQ_INSERT_HEAD(&q->q, p, runqlink);    
  }
  release(&q->lock);
  popcli();
  return p;
}

void
initsched(void)
{
  int i;

  for (i = 0; i < NCPU; i++) {
      initlock(&runq[i].lock, "runq", LOCKSTAT_SCHED);
    STAILQ_INIT(&runq[i].q);
  }
}
