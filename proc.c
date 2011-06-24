#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"
#include "xv6-mtrace.h"

int __attribute__ ((aligned (CACHELINE))) idle[NCPU];
struct ns *nspid __attribute__ ((aligned (CACHELINE)));
struct ns *nsrunq __attribute__ ((aligned (CACHELINE)));
static struct proc *initproc __attribute__ ((aligned (CACHELINE)));

extern void forkret(void);
extern void trapret(void);

enum { sched_debug = 0 };

void
pinit(void)
{
  int c;

  nspid = nsalloc(0);
  if (nspid == 0)
    panic("pinit");

  nsrunq = nsalloc(1);
  if (nsrunq == 0)
    panic("pinit runq");

  for (c = 0; c < NCPU; c++)
    idle[c] = 1;
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

  p = kmalloc(sizeof(struct proc));
  if (p == 0) return 0;
  memset(p, 0, sizeof(*p));
  p->state = EMBRYO;

  p->state = EMBRYO;
  p->pid = ns_allockey(nspid);
  p->epoch = INF;
  p->cpuid = cpu->id;
  p->on_runq = -1;

  snprintf(p->lockname, sizeof(p->lockname), "cv:proc:%d", p->pid);
  initlock(&p->lock, p->lockname+3);
  initcondvar(&p->cv, p->lockname);

  if (ns_insert(nspid, KI(p->pid), (void *) p) < 0)
    panic("allocproc: ns_insert");

  // Allocate kernel stack if possible.
  if((p->kstack = kalloc()) == 0){
    if (ns_remove(nspid, KI(p->pid), p) == 0)
      panic("allocproc: ns_remove");
    rcu_delayed(p, kmfree);
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

//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];
  
  p = allocproc();
  initproc = p;
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
  p->cwd = 0; // forkret will fix in the process's context
  acquire(&p->lock);
  addrun(p);
  p->state = RUNNABLE;
  release(&p->lock);
}

// Grow/shrink current process's memory by n bytes.
// Growing may allocate vmas and physical memory,
// but avoids interfering with any existing vma.
// Assumes vmas around proc->brk are part of the growable heap.
// Shrinking just decreases proc->brk; doesn't deallocate.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
  struct vmap *m = proc->vmap;

  if(n < 0 && 0 - n <= proc->brk){
    proc->brk += n;
    return 0;
  }

  if(n < 0 || n > USERTOP || proc->brk + n > USERTOP)
    return -1;

  acquire(&m->lock);

  // find first unallocated address in brk..brk+n
  uint newstart = proc->brk;
  uint newn = n;
  rcu_begin_read();
  while(newn > 0){
    struct vma *e = vmap_overlap(m, newstart, 1);
    if(e == 0)
      break;
    if(e->va_end >= newstart + newn){
      newstart += newn;
      newn = 0;
      break;
    }
    newn -= e->va_end - newstart;
    newstart = e->va_end;
  }
  rcu_end_read();

  if(newn <= 0){
    // no need to allocate
    proc->brk += n;
    release(&m->lock);
    switchuvm(proc);
    return 0;
  }

  // is there space for newstart..newstart+newn?
  if(vmap_overlap(m, newstart, newn) != 0){
    cprintf("growproc: not enough room in address space; brk %d n %d\n",
            proc->brk, n);
    return -1;
  }

  // would the newly allocated region abut the next-higher
  // vma? we can't allow that, since then a future sbrk()
  // would start to use the next region (e.g. the stack).
  if(vmap_overlap(m, PGROUNDUP(newstart+newn), 1) != 0){
    cprintf("growproc: would abut next vma; brk %d n %d\n",
            proc->brk, n);
    return -1;
  }

  struct vmnode *vmn = vmn_allocpg(PGROUNDUP(newn) / PGSIZE);
  if(vmn == 0){
    release(&m->lock);
    cprintf("growproc: vmn_allocpg failed\n");
    return -1;
  }

  release(&m->lock); // XXX

  if(vmap_insert(m, vmn, newstart) < 0){
    vmn_free(vmn);
    cprintf("growproc: vmap_insert failed\n");
    return -1;
  }

  proc->brk += n;
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
  uint cow = 1;

  //  cprintf("%d: fork\n", proc->pid);

  // Allocate process.
  if((np = allocproc()) == 0)
    return -1;

  if(flags == 0) {
    // Copy process state from p.
    if((np->vmap = vmap_copy(proc->vmap, cow)) == 0){
      kfree(np->kstack);
      np->kstack = 0;
      np->state = UNUSED;
      if (ns_remove(nspid, KI(np->pid), np) == 0)
	panic("fork: ns_remove");
      rcu_delayed(np, kmfree);
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
  pid = np->pid;
  safestrcpy(np->name, proc->name, sizeof(proc->name));
  acquire(&proc->lock);
  SLIST_INSERT_HEAD(&proc->childq, np, child_next);
  release(&proc->lock);

  acquire(&np->lock);
  addrun(np);
  np->state = RUNNABLE;
  release(&np->lock);

  migrate(np);

  //  cprintf("%d: fork done (pid %d)\n", proc->pid, pid);
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
    release(&p->lock);

    acquire(&initproc->lock);
    SLIST_INSERT_HEAD(&initproc->childq, p, child_next);
    release(&initproc->lock);
  }

  // Parent might be sleeping in wait().
  acquire(&proc->lock);

  cv_wakeup(&proc->parent->cv);

  if (wakeupinit)
    cv_wakeup(&initproc->cv); 

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
	  release(&p->lock);	// noone else better be trying to lock p
	  pid = p->pid;
	  SLIST_REMOVE(&proc->childq, p, proc, child_next);
	  release(&proc->lock);
	  kfree(p->kstack);
	  p->kstack = 0;
	  vmap_decref(p->vmap);
	  p->state = UNUSED;
	  if (ns_remove(nspid, KI(p->pid), p) == 0)
	    panic("wait: ns_remove");
	  p->pid = 0;
	  p->parent = 0;
	  p->name[0] = 0;
	  p->killed = 0;
	  rcu_delayed(p, kmfree);
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
migrate(struct proc *p)
{
  int c;

  for (c = 0; c < NCPU; c++) {
    if (c == cpu->id)
      continue;
    if (idle[c]) {    // OK if there is a race
      acquire(&p->lock);
      if (p->state != RUNNABLE) {
	release(&p->lock);
	continue;
      }

      if (sched_debug)
	cprintf("cpu%d: migrate %d to %d\n", cpu->id, p->pid, c);

      delrun(p);
      p->curcycles = 0;
      p->cpuid = c;
      addrun(p);
      idle[c] = 0;

      if (p == proc) {
	proc->state = RUNNABLE;
	sched();
      }

      release(&p->lock);
      return;
    }
  }
}

static void *
steal_cb(void *vk, void *v, void *arg)
{
  struct proc *p = v;
  
  acquire(&p->lock);
  if (p->state != RUNNABLE || p->cpuid == cpu->id) {
    release(&p->lock);
    return 0;
  }

  if (p->curcycles == 0 || p->curcycles > MINCYCTHRESH) {
    if (sched_debug)
      cprintf("cpu%d: steal %d (cycles=%d) from %d\n",
	      cpu->id, p->pid, (int)p->curcycles, p->cpuid);
    delrun(p);
    p->curcycles = 0;
    p->cpuid = cpu->id;
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

//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.

static void *
choose_runnable(void *pp, void *arg)
{
  struct proc *p = pp;
  if (p->state == RUNNABLE)
    return p;
  return 0;
}

void
scheduler(void)
{
  // allocate a fake PID for each scheduler thread
  struct proc *schedp = allocproc();
  if (!schedp)
    panic("scheduler allocproc");

  proc = schedp;

  // Enabling mtrace calls in scheduler generates many mtrace_call_entrys.
  // mtrace_call_set(1, cpu->id);
  mtrace_fcall_register(schedp->pid, (unsigned long)scheduler, 0, mtrace_start);

  for(;;){
    // Enable interrupts on this processor.
    sti();

    struct proc *p = ns_enumerate_key(nsrunq, KI(cpu->id), choose_runnable, 0);
    if (p) {
      acquire(&p->lock);
      if (p->state != RUNNABLE) {
	release(&p->lock);
      } else {
	if (idle[cpu->id])
	  idle[cpu->id] = 0;

	// Switch to chosen process.  It is the process's job
	// to release proc->lock and then reacquire it
	// before jumping back to us.
	proc = p;
	switchuvm(p);
	p->state = RUNNING;
	p->tsc = rdtsc();

	mtrace_fcall_register(schedp->pid, 0, 0, mtrace_pause);
	mtrace_fcall_register(proc->pid, 0, 0, mtrace_resume);
	mtrace_call_set(1, cpu->id);
	swtch(&cpu->scheduler, proc->context);
	mtrace_fcall_register(schedp->pid, 0, 0, mtrace_resume);
	mtrace_call_set(0, cpu->id);
	switchkvm();

	// Process is done running for now.
	// It should have changed its p->state before coming back.
	proc = schedp;
	if (p->state != RUNNABLE)
	  delrun(p);
	release(&p->lock);
      }
    } else {
      if (steal()) {
	if (idle[cpu->id])
	  idle[cpu->id] = 0;
      } else {
	if (!idle[cpu->id])
	  idle[cpu->id] = 1;
      }
    }

    int now = ticks;
    if (now - cpu->last_rcu_gc_ticks > 100) {
      rcu_gc();
      cpu->last_rcu_gc_ticks = now;
    }

    if (idle[cpu->id]) {
      sti();
      hlt();
    }
  }
}

// Enter scheduler.  Must hold only proc->lock
// and have changed proc->state.
void
sched(void)
{
  int intena;

#if SPINLOCK_DEBUG
  if(!holding(&proc->lock))
    panic("sched proc->lock");
#endif
  if(cpu->ncli != 1)
    panic("sched locks");
  if(proc->state == RUNNING)
    panic("sched running");
  if(readeflags()&FL_IF)
    panic("sched interruptible");
  intena = cpu->intena;
  proc->curcycles += rdtsc() - proc->tsc;
  mtrace_fcall_register(proc->pid, 0, 0, mtrace_pause);
  mtrace_call_set(0, cpu->id);

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

  // just for the first process. can't do it earlier
  // b/c file system code needs a process context
  // in which to call cv_sleep().
  if(proc->cwd == 0)
    proc->cwd = namei("/");

  // Return to "caller", actually trapret (see allocproc).
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
  struct proc *p;

  p = (struct proc *) ns_lookup(nspid, KI(pid));
  if (p == 0) {
    panic("kill");
    return -1;
  }
  acquire(&p->lock);
  p->killed = 1;
  if(p->state == SLEEPING){
    // XXX
    // we need to wake p up if it is cv_sleep()ing.
    // can't change p from SLEEPING to RUNNABLE since that
    //   would make some condvar->waiters a dangling reference,
    //   and the non-zero p->cv_next will cause a future panic.
    // can't call cv_wakeup(p->oncv) since that results in
    //   deadlock (addrun() acquires p->lock).
    // can't release p->lock then call cv_wakeup() since the
    //   cv might be deallocated while we're using it
    //   (pipes dynamically allocate condvars).
  }
  release(&p->lock);
  return 0;
}

void *procdump(void *vk, void *v, void *arg)
{
  struct proc *p = (struct proc *) v;

  static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  char *state;

  if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
    state = states[p->state];
  else
    state = "???";
  cprintf("%d %s %s %d, ", p->pid, state, p->name, p->cpuid);

  uint pc[10];
  if(p->state == SLEEPING){
    getcallerpcs((uint*)p->context->ebp+2, pc);
    for(int i=0; i<10 && pc[i] != 0; i++)
      cprintf(" %p", pc[i]);
  }
  cprintf("\n");
  return 0;
}

//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdumpall(void)
{
  ns_enumerate(nspid, procdump, 0);
}
