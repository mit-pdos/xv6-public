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
extern void forkret1(struct Trapframe*);

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
  p->ts.ss0 = SEG_KDATA << 3;
  p->ts.esp0 = (unsigned)(p->kstack + KSTACKSIZE);

  // XXX it may be wrong to modify the current segment table!

  p->gdt[0] = SEG_NULL;
  p->gdt[SEG_KCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, 0);
  p->gdt[SEG_KDATA] = SEG(STA_W, 0, 0xffffffff, 0);
  p->gdt[SEG_TSS] = SEG16(STS_T32A, (unsigned) &p->ts,
                                sizeof(p->ts), 0);
  p->gdt[SEG_TSS].s = 0;
  p->gdt[SEG_UCODE] = SEG(STA_X|STA_R, (unsigned)p->mem, p->sz, 3);
  p->gdt[SEG_UDATA] = SEG(STA_W, (unsigned)p->mem, p->sz, 3);
}

// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and return it.
// Otherwise return 0.
struct proc*
allocproc(void)
{
  int i;
  struct proc *p;
  
  for(i = 0; i < NPROC; i++){
    p = &proc[i];
    if(p->state == UNUSED){
      p->state = EMBRYO;
      return p;
    }
  }
  return 0;
}

// Create a new process copying p as the parent.
// Does not copy the kernel stack.  
// Instead, sets up stack to return as if from system call.
// Caller must arrange for process to run (set state to RUNNABLE).
struct proc *
copyproc(struct proc* p)
{
  int i;
  struct proc *np;

  // Allocate process.
  acquire(&proc_table_lock);
  if((np = allocproc()) == 0){
    release(&proc_table_lock);
    return 0;
  }
  np->pid = next_pid++;
  np->ppid = p->pid;
  release(&proc_table_lock);

  // Copy process image memory.
  np->sz = p->sz;
  np->mem = kalloc(np->sz);
  if(np->mem == 0){
    np->state = UNUSED;
    return 0;
  }
  memmove(np->mem, p->mem, np->sz);

  // Allocate kernel stack.
  np->kstack = kalloc(KSTACKSIZE);
  if(np->kstack == 0){
    kfree(np->mem, np->sz);
    np->state = UNUSED;
    return 0;
  }
  
  // Initialize segment table.
  setupsegs(np);

  // Copy trapframe registers from parent.
  np->tf = (struct Trapframe*)(np->kstack + KSTACKSIZE) - 1;
  *np->tf = *p->tf;
  
  // Clear %eax so that fork system call returns 0 in child.
  np->tf->regs.eax = 0;

  // Set up new jmpbuf to start executing at forkret (see below).
  memset(&np->jmpbuf, 0, sizeof np->jmpbuf);
  np->jmpbuf.eip = (unsigned)forkret;
  np->jmpbuf.esp = (unsigned)np->tf;

  // Copy file descriptors
  for(i = 0; i < NOFILE; i++){
    np->fds[i] = p->fds[i];
    if(np->fds[i])
      fd_incref(np->fds[i]);
  }

  return np;
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
      p->gdt[SEG_TSS].type = STS_T32A;
    
      // XXX should probably have an lgdt() function in x86.h
      // to confine all the inline assembly.
      // XXX probably ought to lgdt on trap return too, in case
      // a system call has moved a program or changed its size.
      lgdt(p->gdt, sizeof p->gdt);
    // asm volatile("lgdt %0" : : "g" (p->gdt_pd.lim));

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
yield(void)
{
  struct proc *p;

  if((p=curproc[cpu()]) == 0 || curproc[cpu()]->state != RUNNING)
    panic("yield");
  acquire(&proc_table_lock);
  p->state = RUNNABLE;
  sched();
  release(&proc_table_lock);
}

// A process's very first scheduling by scheduler()
// will longjmp here to do the first jump into user space.
void
forkret(void)
{
  // Still holding proc_table_lock from scheduler.
  release(&proc_table_lock);
  
  // Jump into assembly, never to return.
  forkret1(curproc[cpu()]->tf);
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
proc_exit(void)
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

