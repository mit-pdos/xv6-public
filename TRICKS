This file lists subtle things that might not be commented 
as well as they should be in the source code and that
might be worth pointing out in a longer explanation or in class.

---

[2009/07/12: No longer relevant; forkret1 changed
and this is now cleaner.]

forkret1 in trapasm.S is called with a tf argument.
In order to use it, forkret1 copies the tf pointer into
%esp and then jumps to trapret, which pops the 
register state out of the trap frame.  If an interrupt
came in between the mov tf, %esp and the iret that
goes back out to user space, the interrupt stack frame
would end up scribbling over the tf and whatever memory
lay under it.

Why is this safe?  Because forkret1 is only called
the first time a process returns to user space, and
at that point, cp->tf is set to point to a trap frame
constructed at the top of cp's kernel stack.  So tf 
*is* a valid %esp that can hold interrupt state.

If other tf's were used in forkret1, we could add
a cli before the mov tf, %esp.

---

In pushcli, must cli() no matter what.  It is not safe to do

  if(cpus[cpu()].ncli == 0)
    cli();
  cpus[cpu()].ncli++;

because if interrupts are off then we might call cpu(), get
rescheduled to a different cpu, look at cpus[oldcpu].ncli,
and wrongly decide not to disable interrupts on the new cpu.

Instead do 

  cli();
  cpus[cpu()].ncli++;

always.

---

There is a (harmless) race in pushcli, which does

	eflags = readeflags();
	cli();
	if(c->ncli++ == 0)
		c->intena = eflags & FL_IF;

Consider a bottom-level pushcli.  
If interrupts are disabled already, then the right thing
happens: read_eflags finds that FL_IF is not set,
and intena = 0.  If interrupts are enabled, then
it is less clear that the right thing happens:
the readeflags can execute, then the process
can get preempted and rescheduled on another cpu,
and then once it starts running, perhaps with 
interrupts disabled (can happen since the scheduler
only enables interrupts once per scheduling loop,
not every time it schedules a process), it will 
incorrectly record that interrupts *were* enabled.
This doesn't matter, because if it was safe to be
running with interrupts enabled before the context
switch, it is still safe (and arguably more correct)
to run with them enabled after the context switch too.

In fact it would be safe if scheduler always set
	c->intena = 1;
before calling swtch, and perhaps it should.

---

The x86's processor-ordering memory model 
matches spin locks well, so no explicit memory
synchronization instructions are required in
acquire and release.  

Consider two sequences of code on different CPUs:

CPU0
A;
release(lk);

and

CPU1
acquire(lk);
B;

We want to make sure that:
  - all reads in B see the effects of writes in A.
  - all reads in A do *not* see the effects of writes in B.
 
The x86 guarantees that writes in A will go out
to memory before the write of lk->locked = 0 in 
release(lk).  It further guarantees that CPU1 
will observe CPU0's write of lk->locked = 0 only
after observing the earlier writes by CPU0.
So any reads in B are guaranteed to observe the
effects of writes in A.

According to the Intel manual behavior spec, the
second condition requires a serialization instruction
in release, to avoid reads in A happening after giving
up lk.  No Intel SMP processor in existence actually
moves reads down after writes, but the language in
the spec allows it.  There is no telling whether future
processors will need it.

---

The code in fork needs to read np->pid before
setting np->state to RUNNABLE.  

	int
	fork(void)
	{
	  ...
	  pid = np->pid;
	  np->state = RUNNABLE;
	  return pid;
	}

After setting np->state to RUNNABLE, some other CPU
might run the process, it might exit, and then it might
get reused for a different process (with a new pid), all
before the return statement.  So it's not safe to just do
"return np->pid;".

This works because proc.h marks the pid as volatile.
