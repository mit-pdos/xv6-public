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

static int __mpalign__ idle[NCPU];
static struct proc *the_idle[NCPU] __mpalign__;

void
idleloop(void)
{
  extern void forkret(void);
  struct proc *idlep = the_idle[cpunum()];

  // Test the work queue
  //extern void testwq(void);
  //testwq();

  // Enabling mtrace calls in scheduler generates many mtrace_call_entrys.
  // mtrace_call_set(1, cpu->id);
  mtstart(scheduler, idlep);

  for(;;){
    // Enable interrupts on this processor.
    sti();

    struct proc *p = schednext();
    if (p) {
      if (get_proc_state(p) != RUNNABLE) {
        panic("Huh?");
      } else {
        if (idle[mycpu()->id])
	  idle[mycpu()->id] = 0;

	// Switch to chosen process.  It is the process's job
	// to release proc->lock and then reacquire it
	// before jumping back to us.
	mycpu()->proc = p;
	switchuvm(p);
	set_proc_state(p, RUNNING);
	p->tsc = rdtsc();

        mtpause(idlep);
        if (p->context->rip != (uptr)forkret && 
            p->context->rip != (uptr)threadstub)
        {
          mtresume(p);
        }
	mtrec();

	swtch(&mycpu()->scheduler, myproc()->context);
        mtresume(idlep);
	mtign();
	switchkvm();

	// Process is done running for now.
	// It should have changed its p->state before coming back.
	mycpu()->proc = idlep;
	if (get_proc_state(p) == RUNNABLE)
	  addrun(p);
	release(&p->lock);
      }
    } else {
      if (steal()) {
	if (idle[mycpu()->id])
	  idle[mycpu()->id] = 0;
      } else {
	if (!idle[mycpu()->id])
	  idle[mycpu()->id] = 1;
      }
    }

    if (idle[mycpu()->id]) {
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
  idle[cpunum()] = 1;
}
