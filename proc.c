#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "x86.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"
#include "xv6-mtrace.h"

struct ptable ptables[NCPU];
struct runq runqs[NCPU];
static struct proc *initproc;

extern void forkret(void);
extern void trapret(void);

void
pinit(void)
{
  int c;
  int i;

  for (c = 0; c < NCPU; c++) {
    ptables[c].nextpid = (c << 16) | (1);
    ptables[c].name[0] = (char) (c + '0');
    safestrcpy(ptables[c].name+1, "ptable", MAXNAME-1);
    initlock(&ptables[c].lock, ptables[c].name);

    for (i = 0; i < NPROC; i++) {
      initlock(&ptables[c].proc[i].lock, ptables[c].proc[i].name);
      initcondvar(&ptables[c].proc[i].cv, ptables[c].proc[i].name);
    }

    runqs[c].name[0] = (char) (c + '0');
    safestrcpy(runqs[c].name+1, "runq", MAXNAME-1);
    initlock(&runqs[c].lock, runqs[c].name);
  }
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
  char *sp;

  acquire(&ptable->lock);
  for(p = ptable->proc; p < &ptable->proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;
  release(&ptable->lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = ptable->nextpid++;
  release(&ptable->lock);

  // Allocate kernel stack if possible.
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;
  
  // Leave room for trap frame.
  sp -= sizeof *p->tf;
  p->tf = (struct trapframe*)sp;
  
  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  return p;
}

static void
addrun1(struct runq *rq, struct proc *p)
{
  struct proc *q;
  SLIST_FOREACH(q, &rq->runq, run_next) {
    if (q == p) {
      cprintf("allready on q\n");
      p->state = RUNNABLE; 
      return;
    }
  }
  p->state = RUNNABLE;   // XXX race?
  SLIST_INSERT_HEAD(&rq->runq, p, run_next);
}

void
addrun(struct proc *p)
{
  acquire(&runq->lock);
  // cprintf("%d: addrun %d\n", cpunum(), p->pid);
  addrun1(runq, p);
  release(&runq->lock);
}

static void 
delrun1(struct runq *rq, struct proc *p)
{
  struct proc *q, *nq;
  SLIST_FOREACH_SAFE(q, &rq->runq, run_next, nq) {
    if (q == p) {
      SLIST_REMOVE(&rq->runq, q, proc, run_next);
      return;
    }
  }
}

void
delrun(struct proc *p)
{
  acquire(&runq->lock);
  // cprintf("%d: delrun %d\n", cpunum(), p->pid);
  delrun1(runq, p);
  release(&runq->lock);
}

//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];
  
  p = allocproc();
  initproc = p;
  if((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  if((p->vmap = vmap_alloc()) == 0)
    panic("userinit: out of vmaps?");
  struct vmnode *vmn = vmn_allocpg(PGROUNDUP((int)_binary_initcode_size) / PGSIZE);
  if(vmn == 0)
    panic("userinit: vmn_allocpg");
  if(vmap_insert(p->vmap, vmn, 0) < 0)
    panic("userinit: vmap_insert");
  if(copyout(p->vmap, 0, _binary_initcode_start, (int)_binary_initcode_size) < 0)
    panic("userinit: copyout");
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
  p->tf->es = p->tf->ds;
  p->tf->ss = p->tf->ds;
  p->tf->eflags = FL_IF;
  p->tf->esp = PGSIZE;
  p->tf->eip = 0;  // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");
  addrun(p);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
  uint brk = proc->brk;
  uint nbrk = brk + n;

  struct vma *vma = vmap_lookup(proc->vmap, brk-1);
  if(vma == 0)
    return -1;

  if(nbrk > vma->va_end){
    /* XXX */
    release(&vma->lock);
    cprintf("cannot resize heap: %d -> %d\n", brk, nbrk);
    return -1;
  }

  proc->brk = brk + n;
  release(&vma->lock);
  switchuvm(proc);
  return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(int flags)
{
  int i, pid;
  struct proc *np;

  // Allocate process.
  if((np = allocproc()) == 0)
    return -1;

  if((np->pgdir = setupkvm()) == 0){
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }

  if(flags == 0) {
    // Copy process state from p.
    if((np->vmap = vmap_copy(proc->vmap)) == 0){
      freevm(np->pgdir);
      kfree(np->kstack);
      np->kstack = 0;
      np->state = UNUSED;
      return -1;
    }
  } else {
    np->vmap = proc->vmap;
    __sync_fetch_and_add(&np->vmap->ref, 1);
  }

  np->brk = proc->brk;
  np->parent = proc;
  *np->tf = *proc->tf;

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;

  for(i = 0; i < NOFILE; i++)
    if(proc->ofile[i])
      np->ofile[i] = filedup(proc->ofile[i]);
  np->cwd = idup(proc->cwd);

  SLIST_INSERT_HEAD(&proc->childq, np, child_next);  // XXX lock?
  pid = np->pid;
  addrun(np);
  safestrcpy(np->name, proc->name, sizeof(proc->name));

  return pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void)
{
  struct proc *p, *np;
  int fd;
  int wakeupinit;

  if(proc == initproc)
    panic("init exiting");

  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
    if(proc->ofile[fd]){
      fileclose(proc->ofile[fd]);
      proc->ofile[fd] = 0;
    }
  }

  iput(proc->cwd);
  proc->cwd = 0;

  // Pass abandoned children to init.
  wakeupinit = 0;
  SLIST_FOREACH_SAFE(p, &proc->childq, child_next, np) {
    acquire(&p->lock);
    p->parent = initproc;
    if(p->state == ZOMBIE)
	  wakeupinit = 1;
    SLIST_REMOVE(&proc->childq, p, proc, child_next);
    SLIST_INSERT_HEAD(&initproc->childq, p, child_next);  // XXX lock?
    release(&p->lock);
  }

  // Parent might be sleeping in wait().
  cv_wakeup(&proc->parent->cv);

  if (wakeupinit)
    cv_wakeup(&initproc->cv); 

  acquire(&proc->lock); 

  delrun(proc);

  // Jump into the scheduler, never to return.
  proc->state = ZOMBIE;
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(void)
{
  struct proc *p, *np;
  int havekids, pid;

  for(;;){
    // Scan children for ZOMBIEs
    havekids = 0;
    acquire(&proc->lock);
    SLIST_FOREACH_SAFE(p, &proc->childq, child_next, np) {
	havekids = 1;
	acquire(&p->lock);
	if(p->state == ZOMBIE){
	  pid = p->pid;
	  SLIST_REMOVE(&proc->childq, p, proc, child_next);
	  kfree(p->kstack);
	  p->kstack = 0;
	  freevm(p->pgdir);
	  vmap_decref(p->vmap);
	  p->state = UNUSED;
	  p->pid = 0;
	  p->parent = 0;
	  p->name[0] = 0;
	  p->killed = 0;
	  release(&p->lock);
	  release(&proc->lock);
	  return pid;
	}
	release(&p->lock);
    }

    // No point waiting if we don't have any children.
    if(!havekids || proc->killed){
      release(&proc->lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    cv_sleep(&proc->cv, &proc->lock);  

    release(&proc->lock);
  }
}

void 
steal(void)
{
  int c;
  struct proc *p;

  for (c = 0; c < NCPU; c++) {
    if (c == cpunum())
      continue;
    acquire(&runqs[c].lock);
    SLIST_FOREACH(p, &runqs[c].runq, run_next) {
      if (p->state == RUNNABLE) {
	//cprintf("%d: steal %d from %d\n", cpunum(), p->pid, c);
	delrun1(&runqs[c], p);
	release(&runqs[c].lock);
	addrun(p);
	return;
      }
    }
    release(&runqs[c].lock);
  }
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
  int pid;

  acquire(&ptable->lock);
  pid = ptable->nextpid++;
  release(&ptable->lock);

  // Enabling mtrace calls in scheduler generates many mtrace_call_entrys.
  // mtrace_call_set(1, cpunum());
  mtrace_fcall_register(pid, (unsigned long)scheduler, 0, mtrace_start);

  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&runq->lock);

    SLIST_FOREACH(p, &runq->runq, run_next) {
      if(p->state != RUNNABLE)
        continue;

      acquire(&p->lock);

      // Switch to chosen process.  It is the process's job
      // to release proc->lock and then reacquire it
      // before jumping back to us.
      proc = p;
      switchuvm(p);
      p->state = RUNNING;

      // Release runq lock only after setting RUNNING, to prevent stealing.
      release(&runq->lock);

      mtrace_fcall_register(pid, 0, 0, mtrace_pause);
      mtrace_fcall_register(proc->pid, 0, 0, mtrace_resume);
      mtrace_call_set(1, cpunum());
      swtch(&cpu->scheduler, proc->context);
      mtrace_fcall_register(pid, 0, 0, mtrace_resume);
      mtrace_call_set(0, cpunum());
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      proc = 0;
      release(&p->lock);

      // Cannot loop: process may have been stolen, run_next is another runq.
      break;
    }

    if(p==0) {
      release(&runq->lock);
      steal();
    }
  }
}

// Enter scheduler.  Must hold only proc->lock
// and have changed proc->state.
void
sched(void)
{
  int intena;

  if(!holding(&proc->lock))
    panic("sched proc->lock");
  if(cpu->ncli != 1)
    panic("sched locks");
  if(proc->state == RUNNING)
    panic("sched running");
  if(readeflags()&FL_IF)
    panic("sched interruptible");
  intena = cpu->intena;

  mtrace_fcall_register(proc->pid, 0, 0, mtrace_pause);
  mtrace_call_set(0, cpunum());
  swtch(&proc->context, cpu->scheduler);
  cpu->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
  acquire(&proc->lock);  //DOC: yieldlock
  proc->state = RUNNABLE; 
  sched();
  release(&proc->lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void)
{
  // Still holding proc->lock from scheduler.
  release(&proc->lock);
  
  // Return to "caller", actually trapret (see allocproc).
}



// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
  struct proc *p;
  int c;

  for (c = 0; c < NCPU; c++) {
    acquire(&ptables[c].lock);
    for(p = ptables[c].proc; p < &ptables[c].proc[NPROC]; p++){
      acquire(&p->lock);
      if(p->pid == pid){
	p->killed = 1;
	// Wake process from sleep if necessary.
	if(p->state == SLEEPING)
	  addrun1(&runqs[c], p);
	release(&p->lock);
	release(&ptables[c].lock);
	return 0;
      }
      acquire(&p->lock);
    }
    release(&ptables[c].lock);
  }
  return -1;
}

//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(int c)
{
  static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  int i;
  struct proc *p;
  struct proc *q;
  char *state;
  uint pc[10];
  
  cprintf("proc table cpu %d\n", c);
  for(p = ptables[c].proc; p < &ptables[c].proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    cprintf("%d %s %s", p->pid, state, p->name);
    if(p->state == SLEEPING){
      getcallerpcs((uint*)p->context->ebp+2, pc);
      for(i=0; i<10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
  }
  cprintf("runq: ");
  SLIST_FOREACH(q, &runqs[c].runq, run_next) {
    if(q->state >= 0 && q->state < NELEM(states) && states[q->state])
      state = states[q->state];
    else
      state = "???";
    cprintf("%d %s %s, ", q->pid, state, q->name);
  }
  cprintf("\n");
}

void
procdumpall(void)
{
  int c;
  for (c = 0; c < NCPU; c++) {
    procdump(c);
  }
}

//// dead code

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
  panic("sleep");
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
}
