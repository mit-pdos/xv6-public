#include "types.h"
#include "mmu.h"
#include "x86.h"
#include "param.h"
#include "file.h"
#include "proc.h"
#include "defs.h"
#include "spinlock.h"

struct spinlock proc_table_lock;

struct proc proc[NPROC];
struct proc *curproc[NCPU];
int next_pid = 1;
extern void forkret(void);
extern void forkret1(struct trapframe*);

void
pinit(void)
{
  initlock(&proc_table_lock, "proc_table");
}

// Set up CPU's segment descriptors and task state for a
// given process.
// If p==0, set up for "idle" state for when scheduler()
// is idling, not running any process.
void
setupsegs(struct proc *p)
{
  struct cpu *c = &cpus[cpu()];

  c->ts.ss0 = SEG_KDATA << 3;
  if(p){
    c->ts.esp0 = (uint)(p->kstack + KSTACKSIZE);
  } else {
    c->ts.esp0 = 0xffffffff;
  }

  c->gdt[0] = SEG_NULL;
  c->gdt[SEG_KCODE] = SEG(STA_X|STA_R, 0, 0x100000 + 64*1024, 0); // xxx
  c->gdt[SEG_KDATA] = SEG(STA_W, 0, 0xffffffff, 0);
  c->gdt[SEG_TSS] = SEG16(STS_T32A, (uint) &c->ts, sizeof(c->ts), 0);
  c->gdt[SEG_TSS].s = 0;
  if(p){
    c->gdt[SEG_UCODE] = SEG(STA_X|STA_R, (uint)p->mem, p->sz, 3);
    c->gdt[SEG_UDATA] = SEG(STA_W, (uint)p->mem, p->sz, 3);
  } else {
    c->gdt[SEG_UCODE] = SEG_NULL;
    c->gdt[SEG_UDATA] = SEG_NULL;
  }

  lgdt(c->gdt, sizeof c->gdt);
  ltr(SEG_TSS << 3);
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
struct proc*
copyproc(struct proc *p)
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

  // Copy user memory.
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
    np->mem = 0;
    np->state = UNUSED;
    return 0;
  }

  // Copy trapframe registers from parent.
  np->tf = (struct trapframe*)(np->kstack + KSTACKSIZE) - 1;
  memmove(np->tf, p->tf, sizeof(*np->tf));

  // Clear %eax so that fork system call returns 0 in child.
  np->tf->eax = 0;

  // Set up new jmpbuf to start executing at forkret (see below).
  memset(&np->jmpbuf, 0, sizeof np->jmpbuf);
  np->jmpbuf.eip = (uint)forkret;
  np->jmpbuf.esp = (uint)np->tf - 4;

  // Copy file descriptors
  for(i = 0; i < NOFILE; i++){
    np->ofile[i] = p->ofile[i];
    if(np->ofile[i])
      fileincref(np->ofile[i]);
  }

  np->cwd = p->cwd;
  iincref(p->cwd);

  return np;
}

// Grow current process's memory by n bytes.
// Return old size on success, -1 on failure.
int
growproc(int n)
{
  struct proc *cp = curproc[cpu()];
  char *newmem, *oldmem;

  newmem = kalloc(cp->sz + n);
  if(newmem == 0)
    return 0xffffffff;
  memmove(newmem, cp->mem, cp->sz);
  memset(newmem + cp->sz, 0, n);
  oldmem = cp->mem;
  cp->mem = newmem;
  kfree(oldmem, cp->sz);
  cp->sz += n;
  return cp->sz - n;
}

//PAGEBREAK: 42
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

  for(;;){
    // Loop over process table looking for process to run.
    acquire(&proc_table_lock);

    for(i = 0; i < NPROC; i++){
      p = &proc[i];
      if(p->state != RUNNABLE)
        continue;

      // Switch to chosen process.  It is the process's job
      // to release proc_table_lock and then reacquire it
      // before jumping back to us.

      setupsegs(p);
      curproc[cpu()] = p;
      p->state = RUNNING;
      if(setjmp(&cpus[cpu()].jmpbuf) == 0)
        longjmp(&p->jmpbuf);

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      curproc[cpu()] = 0;

      setupsegs(0);
    }

    release(&proc_table_lock);
  }
}

// Enter scheduler.  Must already hold proc_table_lock
// and have changed curproc[cpu()]->state.
void
sched(void)
{
  struct proc *p = curproc[cpu()];

  if(setjmp(&p->jmpbuf) == 0)
    longjmp(&cpus[cpu()].jmpbuf);
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
  struct proc *p = curproc[cpu()];

  acquire(&proc_table_lock);
  p->state = RUNNABLE;
  sched();
  release(&proc_table_lock);
}

// A fork child's very first scheduling by scheduler()
// will longjmp here. "return" to user space.
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

  if(lk == 0)
    panic("sleep without lk");

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
    if(cp->ofile[fd]){
      fileclose(cp->ofile[fd]);
      cp->ofile[fd] = 0;
    }
  }

  idecref(cp->cwd);
  cp->cwd = 0;

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
  cp->killed = 0;
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
    // Scan through table looking for zombie children.
    havekids = 0;
    for(i = 0; i < NPROC; i++){
      p = &proc[i];
      if(p->state == UNUSED)
        continue;
      if(p->ppid == cp->pid){
        if(p->state == ZOMBIE){
          // Found one.
          kfree(p->mem, p->sz);
          kfree(p->kstack, KSTACKSIZE);
          pid = p->pid;
          p->state = UNUSED;
          p->pid = 0;
          p->ppid = 0;
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

