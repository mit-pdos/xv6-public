#include "types.h"
#include "mmu.h"
#include "x86.h"
#include "param.h"
#include "fd.h"
#include "proc.h"
#include "defs.h"
#include "spinlock.h"

struct spinlock proc_table_lock;

struct proc proc[NPROC];
struct proc *curproc[NCPU];
int next_pid = 1;
extern void forkret(void);

/*
 * set up a process's task state and segment descriptors
 * correctly, given its current size and address in memory.
 * this should be called whenever the latter change.
 * doesn't change the cpu's current segmentation setup.
 */
void
setupsegs(struct proc *p)
{
  memset(&p->ts, 0, sizeof(struct Taskstate));
  p->ts.ts_ss0 = SEG_KDATA << 3;
  p->ts.ts_esp0 = (unsigned)(p->kstack + KSTACKSIZE);

  // XXX it may be wrong to modify the current segment table!

  p->gdt[0] = SEG_NULL;
  p->gdt[SEG_KCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, 0);
  p->gdt[SEG_KDATA] = SEG(STA_W, 0, 0xffffffff, 0);
  p->gdt[SEG_TSS] = SEG16(STS_T32A, (unsigned) &p->ts,
                                sizeof(p->ts), 0);
  p->gdt[SEG_TSS].sd_s = 0;
  p->gdt[SEG_UCODE] = SEG(STA_X|STA_R, (unsigned)p->mem, p->sz, 3);
  p->gdt[SEG_UDATA] = SEG(STA_W, (unsigned)p->mem, p->sz, 3);
  p->gdt_pd.pd__garbage = 0;
  p->gdt_pd.pd_lim = sizeof(p->gdt) - 1;
  p->gdt_pd.pd_base = (unsigned) p->gdt;
}

extern void trapret();

/*
 * internal fork(). does not copy kernel stack; instead,
 * sets up the stack to return as if from system call.
 * caller must set state to RUNNABLE.
 */
struct proc *
newproc()
{
  struct proc *np;
  struct proc *op;
  int fd;

  acquire(&proc_table_lock);

  for(np = &proc[1]; np < &proc[NPROC]; np++){
    if(np->state == UNUSED){
      np->state = EMBRYO;
      break;
    }
  }
  if(np >= &proc[NPROC]){
    release(&proc_table_lock);
    return 0;
  }

  // copy from proc[0] if we're bootstrapping
  op = curproc[cpu()];
  if(op == 0)
    op = &proc[0];

  np->pid = next_pid++;
  np->ppid = op->pid;

  release(&proc_table_lock);

  np->sz = op->sz;
  np->mem = kalloc(op->sz);
  if(np->mem == 0)
    return 0;
  memcpy(np->mem, op->mem, np->sz);
  np->kstack = kalloc(KSTACKSIZE);
  if(np->kstack == 0){
    kfree(np->mem, op->sz);
    np->state = UNUSED;
    return 0;
  }
  setupsegs(np);
  
  // set up kernel stack to return to user space
  np->tf = (struct Trapframe *) (np->kstack + KSTACKSIZE - sizeof(struct Trapframe));
  *(np->tf) = *(op->tf);
  np->tf->tf_regs.reg_eax = 0; // so fork() returns 0 in child

  // Set up new jmpbuf to start executing forkret (see trapasm.S)
  // with esp pointing at tf.  Forkret will call forkret1 (below) to release
  // the proc_table_lock and then jump into the usual trap return code.
  memset(&np->jmpbuf, 0, sizeof np->jmpbuf);
  np->jmpbuf.jb_eip = (unsigned) forkret;
  np->jmpbuf.jb_esp = (unsigned) np->tf - 4;  // -4 for the %eip that isn't actually there

  // Copy file descriptors
  for(fd = 0; fd < NOFILE; fd++){
    np->fds[fd] = op->fds[fd];
    if(np->fds[fd])
      fd_reference(np->fds[fd]);
  }

  return np;
}

void
forkret1(void)
{
  release(&proc_table_lock);
}

// Per-CPU process scheduler. 
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - longjmp to start running that process
//  - eventually that process transfers control back
//      via longjmp back to the top of scheduler.
void
scheduler(void)
{
  struct proc *p;
  int i;

  cprintf("start scheduler on cpu %d jmpbuf %p\n", cpu(), &cpus[cpu()].jmpbuf);
  cpus[cpu()].lastproc = &proc[0];

  for(;;){
    // Loop over process table looking for process to run.
    acquire(&proc_table_lock);
    for(i = 0; i < NPROC; i++){
      p = &proc[i];
      if(p->state != RUNNABLE)
        continue;
      
      // Run this process.
      // XXX move this into swtch or trapret or something.
      // It can run on the other stack.
      // h/w sets busy bit in TSS descriptor sometimes, and faults
      // if it's set in LTR. so clear tss descriptor busy bit.
      p->gdt[SEG_TSS].sd_type = STS_T32A;
    
      // XXX should probably have an lgdt() function in x86.h
      // to confine all the inline assembly.
      // XXX probably ought to lgdt on trap return too, in case
      // a system call has moved a program or changed its size.
      asm volatile("lgdt %0" : : "g" (p->gdt_pd.pd_lim));
      ltr(SEG_TSS << 3);

      // Switch to chosen process.  It is the process's job 
      // to release proc_table_lock and then reacquire it
      // before jumping back to us.
      if(0) cprintf("cpu%d: run %d\n", cpu(), p-proc);
      curproc[cpu()] = p;
      p->state = RUNNING;
      if(setjmp(&cpus[cpu()].jmpbuf) == 0)
        longjmp(&p->jmpbuf);
    
      // Process is done running for now. 
      // It should have changed its p->state before coming back.
      curproc[cpu()] = 0;
      if(p->state == RUNNING)
        panic("swtch to scheduler with state=RUNNING");
      
      // XXX if not holding proc_table_lock panic.
    }
    release(&proc_table_lock);
    
    if(cpus[cpu()].nlock != 0)
      panic("holding locks in scheduler");

    // With proc_table_lock released, there are no 
    // locks held on this cpu, so interrupts are enabled.
    // Hardware interrupts can happen here.
    // Also, releasing the lock here lets the other CPUs
    // look for runnable processes too.
  }
}

// Enter scheduler.  Must already hold proc_table_lock
// and have changed curproc[cpu()]->state.
void
sched(void)
{
  if(setjmp(&curproc[cpu()]->jmpbuf) == 0)
    longjmp(&cpus[cpu()].jmpbuf);
}

// Give up the CPU for one scheduling round.
void
yield()
{
  struct proc *p;

  if((p=curproc[cpu()]) == 0 || curproc[cpu()]->state != RUNNING)
    panic("yield");
  acquire(&proc_table_lock);
  p->state = RUNNABLE;
  sched();
  release(&proc_table_lock);
}

// Atomically release lock and sleep on chan.
// Reacquires lock when reawakened.
void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = curproc[cpu()];

  if(p == 0)
    panic("sleep");

  // Must acquire proc_table_lock in order to 
  // change p->state and then call sched.
  // Once we hold proc_table_lock, we can be 
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with proc_table_lock locked),
  // so it's okay to release lk.
  if(lk != &proc_table_lock){
    acquire(&proc_table_lock);
    release(lk);
  }

  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;
  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  if(lk != &proc_table_lock){
    release(&proc_table_lock);
    acquire(lk);
  }
}

// Wake up all processes sleeping on chan.
// Proc_table_lock must be held.
void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = proc; p < &proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan)
      p->state = RUNNABLE;
}

// Wake up all processes sleeping on chan.
// Proc_table_lock is acquired and released.
void
wakeup(void *chan)
{
  acquire(&proc_table_lock);
  wakeup1(chan);
  release(&proc_table_lock);
}

// Kill the process with the given pid.
// Process won't actually exit until it returns
// to user space (see trap in trap.c).
int
proc_kill(int pid)
{
  struct proc *p;

  acquire(&proc_table_lock);
  for(p = proc; p < &proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        p->state = RUNNABLE;
      release(&proc_table_lock);
      return 0;
    }
  }
  release(&proc_table_lock);
  return -1;
}

// Exit the current process.  Does not return.
// Exited processes remain in the zombie state 
// until their parent calls wait() to find out they exited.
void
proc_exit()
{
  struct proc *p;
  struct proc *cp = curproc[cpu()];
  int fd;

  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
    if(cp->fds[fd]){
      fd_close(cp->fds[fd]);
      cp->fds[fd] = 0;
    }
  }

  acquire(&proc_table_lock);

  // Wake up our parent.
  for(p = proc; p < &proc[NPROC]; p++)
    if(p->pid == cp->ppid)
      wakeup1(p);

  // Reparent our children to process 1.
  for(p = proc; p < &proc[NPROC]; p++)
    if(p->ppid == cp->pid)
      p->ppid = 1;
  
  // Jump into the scheduler, never to return.
  cp->state = ZOMBIE;
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
proc_wait(void)
{
  struct proc *p;
  struct proc *cp = curproc[cpu()];
  int i, havekids, pid;

  acquire(&proc_table_lock);
  for(;;){
    // Scan through table looking zombie children.
    havekids = 0;
    for(i = 0; i < NPROC; i++){
      p = &proc[i];
      if(p->ppid == cp->pid){
        if(p->state == ZOMBIE){
          // Found one.
          kfree(p->mem, p->sz);
          kfree(p->kstack, KSTACKSIZE);
          pid = p->pid;
          p->state = UNUSED;
          p->pid = 0;
          release(&proc_table_lock);
          return pid;
        }
        havekids = 1;
      }
    }
    
    // No point waiting if we don't have any children.
    if(!havekids){
      release(&proc_table_lock);
      return -1;
    }
    
    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(cp, &proc_table_lock);
  }
}

