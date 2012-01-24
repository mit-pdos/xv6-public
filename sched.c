#include "types.h"
#include "kernel.h"
#include "mmu.h"
#include "amd64.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"
#include "cpu.h"
#include "bits.h"
#include "kmtrace.h"
#include "vm.h"
#include "sched.h"
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
    acquire(&q->lock);
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
    initlock(&runq[i].lock, "runq");
    STAILQ_INIT(&runq[i].q);
  }
}

#if 0
// XXX(sbw) This code is broken.  It essentially implements "LIFO"
// scheduling, which leads to starvation and deadlock (in userspace).

struct ns *nsrunq __mpalign__;

// Mark a process RUNNABLE and add it to the runq
// of its cpu. Caller must hold p->lock so that
// some other core doesn't start running the
// process before the caller has finished setting
// the process up, and to cope with racing callers
// e.g. two wakeups on same process. and to
// allow atomic addrun(); sched();
void
addrun(struct proc *p)
{
#if SPINLOCK_DEBUG
  if(!holding(&p->lock))
    panic("addrun no p->lock");
#endif

  if (p->on_runq >= 0)
    panic("addrun on runq already");
  ns_insert(nsrunq, KI(p->cpuid), p);
  p->on_runq = p->cpuid;
}

void
delrun(struct proc *p)
{
#if SPINLOCK_DEBUG
  if(!holding(&p->lock))
    panic("delrun no p->lock");
#endif

  if (p->on_runq < 0)
    panic("delrun not on runq");
  if (ns_remove(nsrunq, KI(p->on_runq), p) == 0)
    panic("delrun: ns_remove");
  p->on_runq = -1;
}

static void *
steal_cb(void *vk, void *v, void *arg)
{
  struct proc *p = v;
  
  acquire(&p->lock);
  if (p->state != RUNNABLE || p->cpuid == mycpu()->id || p->cpu_pin) {
    release(&p->lock);
    return 0;
  }

  if (p->curcycles == 0 || p->curcycles > VICTIMAGE) {
    if (sched_debug)
      cprintf("cpu%d: steal %d (cycles=%d) from %d\n",
	      mycpu()->id, p->pid, (int)p->curcycles, p->cpuid);
    delrun(p);
    p->curcycles = 0;
    p->cpuid = mycpu()->id;
    addrun(p);
    release(&p->lock);
    return p;
  }

  release(&p->lock);
  return 0;
}

int
steal(void)
{
  void *stole = ns_enumerate(nsrunq, steal_cb, 0);
  return stole ? 1 : 0;
}

static void *
choose_runnable(void *pp, void *arg)
{
  struct proc *head = pp;
  
  if (head->state == RUNNABLE)
    return head;
  return 0;
}

struct proc *
schednext(void)
{
  // XXX(sbw) Yikes!  If two processes are runnable on the same
  // CPU this will always return to process addrun inserted last 
  // into the ns.
  return ns_enumerate_key(nsrunq, KI(mycpu()->id), choose_runnable, 0);
}

void
initsched(void)
{
  nsrunq = nsalloc(1);
  if (nsrunq == 0)
    panic("pinit runq");
}
#endif
