#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

struct cpu cpus[NCPU];

struct proc *initproc;

int nextpid = 1;
extern void forkret(void);

// for returning  out of the kernel
extern void sysexit(void);

static void wakeup1(struct proc *chan);

extern char trampout[]; // trampoline.S

void
procinit(void)
{
  initlock(&ptable.lock, "ptable");
}

// Must be called with interrupts disabled,
// to prevent race with process being moved
// to a different CPU.
int
cpuid()
{
  int id = r_tp();
  return id;
}

// Return this core's cpu struct.
// Interrupts must be disabled.
struct cpu*
mycpu(void) {
  int id = cpuid();
  struct cpu *c = &cpus[id];
  return c;
}

// Return the current struct proc *.
struct proc*
myproc(void) {
  push_off();
  struct cpu *c = mycpu();
  struct proc *p = c->proc;
  pop_off();
  return p;
}

//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc*
allocproc(void)
{
  struct proc *p;

  acquire(&ptable.lock);

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;

found:
  initlock(&p->lock, "proc");
  acquire(&p->lock);
  release(&ptable.lock);
  
  p->state = EMBRYO;
  p->pid = nextpid++;

  // Allocate a page for the kernel stack.
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }

  // Allocate a trapframe page.
  if((p->tf = (struct trapframe *)kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }

  // An empty user page table.
  p->pagetable = proc_pagetable(p);

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, sizeof p->context);
  p->context.ra = (uint64)forkret;
  p->context.sp = (uint64)p->kstack + PGSIZE;

  return p;
}

// free a proc structure and the data hanging from it,
// including user pages.
// the proc lock must be held.
static void
freeproc(struct proc *p)
{
  if(p->kstack)
    kfree(p->kstack);
  p->kstack = 0;
  if(p->tf)
    kfree((void*)p->tf);
  p->tf = 0;
  if(p->pagetable)
    proc_freepagetable(p->pagetable, p->sz);
  p->pagetable = 0;
  p->sz = 0;
  p->pid = 0;
  p->parent = 0;
  p->name[0] = 0;
  p->chan = 0;
  p->killed = 0;
  p->state = UNUSED;
}

// Create a page table for a given process,
// with no users pages, but with trampoline pages.
// Called both when creating a process, and
// by exec() when building tentative new memory image,
// which might fail.
pagetable_t
proc_pagetable(struct proc *p)
{
  pagetable_t pagetable;

  // An empty user page table.
  pagetable = uvmcreate();

  // map the trampoline code (for system call return)
  // at the highest user virtual address.
  // only the supervisor uses it, on the way
  // to/from user space, so not PTE_U.
  mappages(pagetable, TRAMPOLINE, PGSIZE,
           (uint64)trampout, PTE_R | PTE_X);

  // map the trapframe, for trampoline.S.
  mappages(pagetable, (TRAMPOLINE - PGSIZE), PGSIZE,
           (uint64)(p->tf), PTE_R | PTE_W);

  return pagetable;
}

// Free a process's page table, and free the
// physical memory the page table refers to.
// Called both when a process exits and from
// exec() if it fails.
void
proc_freepagetable(pagetable_t pagetable, uint64 sz)
{
  unmappages(pagetable, TRAMPOLINE, PGSIZE, 0);
  unmappages(pagetable, TRAMPOLINE-PGSIZE, PGSIZE, 0);
  if(sz > 0)
    uvmfree(pagetable, sz);
}

// a user program that calls exec("/init")
// od -t xC initcode
uchar initcode[] = {
  0x17, 0x05, 0x00, 0x00, 0x13, 0x05, 0x05, 0x02, 0x97, 0x05, 0x00, 0x00, 0x93, 0x85, 0x05, 0x02,
  0x9d, 0x48, 0x73, 0x00, 0x00, 0x00, 0x89, 0x48, 0x73, 0x00, 0x00, 0x00, 0xef, 0xf0, 0xbf, 0xff,
  0x2f, 0x69, 0x6e, 0x69, 0x74, 0x00, 0x00, 0x01, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00
};

//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
  struct proc *p;

  p = allocproc();
  initproc = p;
  
  uvminit(p->pagetable, initcode, sizeof(initcode));
  p->sz = PGSIZE;

  // prepare for the very first kernel->user.
  p->tf->epc = 0;
  p->tf->sp = PGSIZE;

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  p->state = RUNNABLE;

  release(&p->lock);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
  uint sz;
  struct proc *p = myproc();

  acquire(&p->lock);

  sz = p->sz;
  if(n > 0){
    if((sz = uvmalloc(p->pagetable, sz, sz + n)) == 0) {
      release(&p->lock);
      return -1;
    }
  } else if(n < 0){
    if((sz = uvmdealloc(p->pagetable, sz, sz + n)) == 0) {
      release(&p->lock);
      return -1;
    }
  }
  p->sz = sz;
  release(&p->lock);
  return 0;
}

// Create a new process, copying p as the parent.
// Sets up child kernel stack to return as if from system call.
int
fork(void)
{
  int i, pid;
  struct proc *np;
  struct proc *p = myproc();

  // Allocate process.
  if((np = allocproc()) == 0){
    return -1;
  }

  // Copy user memory from parent to child.
  if(uvmcopy(p->pagetable, np->pagetable, p->sz) < 0){
    freeproc(np);
    return -1;
  }
  np->sz = p->sz;

  np->parent = p;

  // copy saved user registers.
  *(np->tf) = *(p->tf);

  // Cause fork to return 0 in the child.
  np->tf->a0 = 0;

  // increment reference counts on open file descriptors.
  for(i = 0; i < NOFILE; i++)
    if(p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]);
  np->cwd = idup(p->cwd);

  safestrcpy(np->name, p->name, sizeof(p->name));

  pid = np->pid;

  np->state = RUNNABLE;

  release(&np->lock);

  return pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait().
void
exit(void)
{
  struct proc *p = myproc();
  int fd;

  if(p == initproc)
    panic("init exiting");

  acquire(&p->lock);
  
  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
    if(p->ofile[fd]){
      struct file *f = p->ofile[fd];
      release(&p->lock);
      
      fileclose(f);
      
      acquire(&p->lock);
      p->ofile[fd] = 0;
    }
  }

  struct inode *cwd = p->cwd;
  release(&p->lock);
  
  begin_op();
  iput(cwd);
  end_op();

  acquire(&p->lock);
  p->cwd = 0;

  // Jump into the scheduler, never to return.
  p->state = ZOMBIE;
  sched();
  panic("zombie exit");
}

void reparent(struct proc *p) {
  struct proc *pp;
  struct proc *parent = p->parent;

  acquire(&parent->lock);
  
  // Parent might be sleeping in wait().
  wakeup1(parent);

  // Pass p's abandoned children to init.
  for(pp = ptable.proc; pp < &ptable.proc[NPROC]; pp++){
    if(pp->parent == p){
      pp->parent = initproc;
      if(pp->state == ZOMBIE) {
          acquire(&initproc->lock);
          wakeup1(initproc);
          acquire(&initproc->lock);
      }
    }
  }

  release(&parent->lock);
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(void)
{
  struct proc *np;
  int havekids, pid;
  struct proc *p = myproc();

  acquire(&p->lock);
  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(np = ptable.proc; np < &ptable.proc[NPROC]; np++){
      if(np->parent != p)
        continue;
      acquire(&np->lock);
      havekids = 1;
      if(np->state == ZOMBIE){
        // Found one.
        pid = np->pid;
        freeproc(np);
        release(&np->lock);
        release(&p->lock);
        return pid;
      }
      release(&np->lock);
    }

    // No point waiting if we don't have any children.
    if(!havekids || p->killed){
      release(&p->lock);
      return -1;
    }
    
    // Wait for children to exit.  (See wakeup1 call in reparent.)
    sleep(p, &p->lock);  //DOC: wait-sleep
  }
}

// Loop over process table looking for process to run.
struct proc *find_runnable(int start) {
  struct proc *p;
  acquire(&ptable.lock);
  for(int i = start; i < start+NPROC; i++) {
    p = &ptable.proc[i % NPROC];
    acquire(&p->lock);
    if(p->state == RUNNABLE) {
      p->state = RUNNING;
      release(&ptable.lock);
      return p;
    }
    release(&p->lock);
  }
  release(&ptable.lock);
  return 0;
}
  
//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();
  int next = 0;
  
  c->proc = 0;
  for(;;){
    // Enable interrupts on this processor.
    intr_on();

    if((p = find_runnable(next)) != 0) {
      next = (next + 1) & NPROC;
      // Switch to chosen process.  It is the process's job
      // to release its lock and then reacquire it
      // before jumping back to us.
      c->proc = p;
      swtch(&c->scheduler, &p->context);

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
      release(&p->lock);
      if(p->state == ZOMBIE) {
        reparent(p);
      }
    }
  }
}

// Enter scheduler.  Must hold only p->lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->noff, but that would
// break in the few places where a lock is held but
// there's no process.
void
sched(void)
{
  int intena;
  struct proc *p = myproc();

  if(!holding(&p->lock))
    panic("sched p->lock");
  if(mycpu()->noff != 1)
    panic("sched locks");
  if(p->state == RUNNING)
    panic("sched running");
  if(intr_get())
    panic("sched interruptible");

  intena = mycpu()->intena;
  swtch(&p->context, &mycpu()->scheduler);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
  acquire(&myproc()->lock);  //DOC: yieldlock
  myproc()->state = RUNNABLE;
  sched();
  release(&myproc()->lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch to forkret.
void
forkret(void)
{
  static int first = 1;

  // Still holding p->lock from scheduler.
  release(&myproc()->lock);

  if (first) {
    // Some initialization functions must be run in the context
    // of a regular process (e.g., they call sleep), and thus cannot
    // be run from main().
    first = 0;
    iinit(ROOTDEV);
    initlog(ROOTDEV);
  }

  usertrapret();
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  
  if(p == 0)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");

  // Must acquire p->lock in order to
  // change p->state and then call sched.
  // Once we hold p->lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with p->lock locked),
  // so it's okay to release lk.
  if(lk != &p->lock){  //DOC: sleeplock0
    acquire(&p->lock);  //DOC: sleeplock1
    release(lk);
  }
  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  if(lk != &p->lock){  //DOC: sleeplock2
    release(&p->lock);
    acquire(lk);
  }
}

//PAGEBREAK!
// Wake up all processes sleeping on chan,
// where chan is a proc, which is locked.
static void
wakeup1(struct proc *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p == chan && p->state == SLEEPING && p->chan == chan) {
      if(p->state != SLEEPING || p->chan != chan)
        panic("wakeup1");
      p->state = RUNNABLE;
    }
}

// Wake up all processes sleeping on chan. Never
// called when holding a p->lock
void
wakeup(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan) {
      acquire(&p->lock);
      if(p->state != SLEEPING || p->chan != chan)
        panic("wakeup");
      p->state = RUNNABLE;
      release(&p->lock);
    }
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      acquire(&p->lock);
      if(p->pid != pid)
        panic("kill");
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        p->state = RUNNABLE;
      release(&p->lock);
      return 0;
    }
  }
  return -1;
}

// Copy to either a user address, or kernel address,
// depending on usr_dst.
// Returns 0 on success, -1 on error.
int
either_copyout(int user_dst, uint64 dst, void *src, uint64 len)
{
  struct proc *p = myproc();
  if(user_dst){
    return copyout(p->pagetable, dst, src, len);
  } else {
    memmove((char *)dst, src, len);
    return 0;
  }
}

// Copy from either a user address, or kernel address,
// depending on usr_src.
// Returns 0 on success, -1 on error.
int
either_copyin(void *dst, int user_src, uint64 src, uint64 len)
{
  struct proc *p = myproc();
  if(user_src){
    return copyin(p->pagetable, dst, src, len);
  } else {
    memmove(dst, (char*)src, len);
    return 0;
  }
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
  static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  struct proc *p;
  char *state;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    printf("%d %s %s", p->pid, state, p->name);
    printf("\n");
  }
}

