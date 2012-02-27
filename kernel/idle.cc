#include "types.h"
#include "kernel.hh"
#include "amd64.h"
#include "spinlock.h"
#include "condvar.h"
#include "proc.hh"
#include "cpu.hh"
#include "sched.hh"

struct proc *idlep[NCPU] __mpalign__;

void
idleloop(void)
{
  // Test the work queue
  //extern void testwq(void);
  //testwq();

  extern void benchwq(void);
  benchwq();

  // Enabling mtrace calls in scheduler generates many mtrace_call_entrys.
  // mtrace_call_set(1, cpu->id);
  //mtstart(scheduler, idlep);

  sti();
  for (;;) {
    acquire(&myproc()->lock);
    myproc()->set_state(RUNNABLE);
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
  idlep[cpunum()] = p;
}
