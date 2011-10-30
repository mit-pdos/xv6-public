#include "types.h"
#include "kernel.h"
#include "param.h"
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

enum { sched_debug = 0 };

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

  if (p->curcycles == 0 || p->curcycles > MINCYCTHRESH) {
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
  return ns_enumerate_key(nsrunq, KI(mycpu()->id), choose_runnable, 0);
}

void
initsched(void)
{
  nsrunq = nsalloc(1);
  if (nsrunq == 0)
    panic("pinit runq");
}
