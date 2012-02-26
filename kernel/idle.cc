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
#include "sched.hh"
#include "kalloc.hh"
#include "vm.hh"
#include "ns.hh"

static struct proc *the_idle[NCPU] __mpalign__;

extern void
forkret(void);

void
post_swtch(void)
{
  if (get_proc_state(mycpu()->prev) == RUNNABLE && 
      mycpu()->prev != the_idle[mycpu()->id])
    addrun(mycpu()->prev);
  release(&mycpu()->prev->lock);
  popcli();
}

void
sched(void)
{
  int intena;

#if SPINLOCK_DEBUG
  if(!holding(&myproc()->lock))
    panic("sched proc->lock");
#endif
  if(mycpu()->ncli != 1)
    panic("sched locks");
  if(get_proc_state(myproc()) == RUNNING)
    panic("sched running");
  if(readrflags()&FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
  myproc()->curcycles += rdtsc() - myproc()->tsc;
  if (get_proc_state(myproc()) == ZOMBIE)
    mtstop(myproc());
  else
    mtpause(myproc());
  mtign();

  struct proc *next = schednext();
  if (next) {
  switchit:
      pushcli();
      if (get_proc_state(next) != RUNNABLE)
        panic("non-RUNNABLE next %s %u", next->name, get_proc_state(next));

      // Switch to chosen process.  It is the process's job
      // to release proc->lock and then reacquire it
      // before jumping back to us.
      struct proc *prev = myproc();
      mycpu()->proc = next;
      mycpu()->prev = prev;

      switchuvm(next);
      set_proc_state(next, RUNNING);
      next->tsc = rdtsc();

      mtpause(next);
      if (next->context->rip != (uptr)forkret && 
          next->context->rip != (uptr)threadstub)
      {
        mtresume(next);
      }
      mtrec();

      swtch(&prev->context, next->context);
      mycpu()->intena = intena;
      post_swtch();
  } else if (get_proc_state(myproc()) != RUNNABLE) {
    next = the_idle[mycpu()->id];
    goto switchit;
  } else {
    set_proc_state(myproc(), RUNNING);
    mycpu()->intena = intena;
    release(&myproc()->lock);
  }

  //swtch(&myproc()->context, mycpu()->scheduler);
  //mycpu()->intena = intena;
}

void
idleloop(void)
{
  // Test the work queue
  //extern void testwq(void);
  //testwq();

  // Enabling mtrace calls in scheduler generates many mtrace_call_entrys.
  // mtrace_call_set(1, cpu->id);
  //mtstart(scheduler, idlep);


  sti();
  for (;;) {
    acquire(&myproc()->lock);
    set_proc_state(myproc(), RUNNABLE);
    sched();

    if (steal() == 0) {
      int worked;
      do {
        assert(mycpu()->ncli == 0);
        worked = wq_trywork();
      } while(worked);
      sti();
    }
  }
}

void
initidle(void)
{
  // allocate a fake PID for each scheduler thread
  struct proc *p = allocproc();
  if (!p)
    panic("initidle allocproc");

  snprintf(p->name, sizeof(p->name), "idle_%u", cpunum());
  mycpu()->proc = p;
  myproc()->cpu_pin = 1;
  the_idle[cpunum()] = p;
}
