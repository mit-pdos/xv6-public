#include "types.h"
#include "kernel.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"
#include "cpu.h"
#include "bits.h"
#include "xv6-mtrace.h"

extern void trapret(void);

int __mpalign__ idle[NCPU];
struct ns *nspid __mpalign__;
struct ns *nsrunq __mpalign__;
static struct proc *bootproc __mpalign__;

enum { sched_debug = 0 };

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
  if(myproc()->state == RUNNING)
    panic("sched running");
  if(readrflags()&FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
  myproc()->curcycles += rdtsc() - myproc()->tsc;
  if (myproc()->state == ZOMBIE)
    mtrace_kstack_stop(myproc());
  else
    mtrace_kstack_pause(myproc());
  mtrace_call_set(0, mycpu()->id);

  swtch(&myproc()->context, mycpu()->scheduler);
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

void
migrate(struct proc *p)
{
  int c;

  for (c = 0; c < NCPU; c++) {
    if (c == mycpu()->id)
      continue;
    if (idle[c]) {    // OK if there is a race
      acquire(&p->lock);
      if (p->state != RUNNABLE || p->cpu_pin) {
	release(&p->lock);
	continue;
      }

      if (sched_debug)
	cprintf("cpu%d: migrate %d to %d\n", mycpu()->id, p->pid, c);

      delrun(p);
      p->curcycles = 0;
      p->cpuid = c;
      addrun(p);
      idle[c] = 0;

      if (p == myproc()) {
        myproc()->state = RUNNABLE;
	sched();
      }

      release(&p->lock);
      return;
    }
  }
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
static void
forkret(void)
{
  // Still holding proc->lock from scheduler.
  release(&myproc()->lock);

  // Just for the first process. can't do it earlier
  // b/c file system code needs a process context
  // in which to call cv_sleep().
  if(myproc()->cwd == 0) {
    mtrace_kstack_start(forkret, myproc());
    myproc()->cwd = namei("/");
    mtrace_kstack_stop(myproc());
  }

  // Return to "caller", actually trapret (see allocproc).
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

  if(myproc() == bootproc)
    panic("init exiting");

  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
    if(myproc()->ofile[fd]){
      fileclose(myproc()->ofile[fd]);
      myproc()->ofile[fd] = 0;
    }
  }

  iput(myproc()->cwd);
  myproc()->cwd = 0;

  // Pass abandoned children to init.
  wakeupinit = 0;
  SLIST_FOREACH_SAFE(p, &(myproc()->childq), child_next, np) {
    acquire(&p->lock);
    p->parent = bootproc;
    if(p->state == ZOMBIE)
      wakeupinit = 1;
    SLIST_REMOVE(&(myproc()->childq), p, proc, child_next);
    release(&p->lock);

    acquire(&bootproc->lock);
    SLIST_INSERT_HEAD(&bootproc->childq, p, child_next);
    release(&bootproc->lock);
  }

  // Parent might be sleeping in wait().
  acquire(&(myproc()->lock));

  cv_wakeup(&(myproc()->parent->cv));

  if (wakeupinit)
    cv_wakeup(&bootproc->cv); 

  // Jump into the scheduler, never to return.
  myproc()->state = ZOMBIE;
  sched();
  panic("zombie exit");
}

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
  p->pid = ns_allockey(nspid);
  p->epoch = INF;
  p->cpuid = mycpu()->id;
  p->on_runq = -1;
  p->cpu_pin = 0;
#if MTRACE
  p->mtrace_stacks.curr = -1;
#endif

  snprintf(p->lockname, sizeof(p->lockname), "cv:proc:%d", p->pid);
  initlock(&p->lock, p->lockname+3);
  initcondvar(&p->cv, p->lockname);

  if (ns_insert(nspid, KI(p->pid), (void *) p) < 0)
    panic("allocproc: ns_insert");

  // Allocate kernel stack if possible.
  if((p->kstack = ksalloc()) == 0){
    if (ns_remove(nspid, KI(p->pid), p) == 0)
      panic("allocproc: ns_remove");
    rcu_delayed(p, kmfree);
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;
  
  // Leave room for trap frame.
  sp -= sizeof *p->tf;
  p->tf = (struct trapframe*)sp;

  // amd64 ABI mandates sp % 16 == 0 before a call instruction
  // (or after executing a ret instruction)
  if ((uptr) sp % 16)
    panic("allocproc: misaligned sp");

  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 8;
  *(u64*)sp = (u64)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->rip = (uptr)forkret;

  return p;
}

// Set up first user process.
void
inituser(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];
  
  p = allocproc();
  bootproc = p;
  if((p->vmap = vmap_alloc()) == 0)
    panic("userinit: out of vmaps?");
  struct vmnode *vmn = 
    vmn_allocpg(PGROUNDUP((uptr)_binary_initcode_size) / PGSIZE);
  if(vmn == 0)
    panic("userinit: vmn_allocpg");
  if(vmap_insert(p->vmap, vmn, 0) < 0)
    panic("userinit: vmap_insert");
  if(copyout(p->vmap, 0, _binary_initcode_start, (uptr)_binary_initcode_size) < 0)
    panic("userinit: copyout");
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = UCSEG | 0x3;
  p->tf->ds = UDSEG | 0x3;
  p->tf->ss = p->tf->ds;
  p->tf->rflags = FL_IF;
  p->tf->rsp = PGSIZE;
  p->tf->rip = 0x0;  // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = 0; // forkret will fix in the process's context
  acquire(&p->lock);
  addrun(p);
  p->state = RUNNABLE;
  release(&p->lock);

  for (u32 c = 0; c < NCPU; c++) {
    struct proc *rcup = allocproc();
    rcup->vmap = vmap_alloc();
    rcup->context->rip = (u64) rcu_gc_worker;
    rcup->cwd = 0;
    rcup->cpuid = c;
    rcup->cpu_pin = 1;

    acquire(&rcup->lock);
    rcup->state = RUNNABLE;
    addrun(rcup);
    release(&rcup->lock);
  }
}

void
initproc(void)
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

static void *
steal_cb(void *vk, void *v, void *arg)
{
  struct proc *p = v;
  
  acquire(&p->lock);
  if (p->state != RUNNABLE || p->cpuid == mycpu()->id || p->cpu_pin) {
    release(&p->lock);
    return 0;
  }

  if (p->curcycles == 0 || p->curcycles > MINCYCTHRESH) {
    if (sched_debug)
      cprintf("cpu%d: steal %d (cycles=%d) from %d\n",
	      mycpu()->id, p->pid, (int)p->curcycles, p->cpuid);
    delrun(p);
    p->curcycles = 0;
    p->cpuid = mycpu()->id;
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

void
scheduler(void)
{
  // allocate a fake PID for each scheduler thread
  struct proc *schedp = allocproc();
  if (!schedp)
    panic("scheduler allocproc");

  mycpu()->proc = schedp;
  myproc()->cpu_pin = 1;

  // Enabling mtrace calls in scheduler generates many mtrace_call_entrys.
  // mtrace_call_set(1, cpu->id);
  mtrace_kstack_start(scheduler, schedp);

  for(;;){
    // Enable interrupts on this processor.
    sti();

    struct proc *p = ns_enumerate_key(nsrunq, KI(mycpu()->id), choose_runnable, 0);
    if (p) {
      acquire(&p->lock);
      if (p->state != RUNNABLE) {
	release(&p->lock);
      } else {
        if (idle[mycpu()->id])
	  idle[mycpu()->id] = 0;

	// Switch to chosen process.  It is the process's job
	// to release proc->lock and then reacquire it
	// before jumping back to us.
	mycpu()->proc = p;
	switchuvm(p);
	p->state = RUNNING;
	p->tsc = rdtsc();

        mtrace_kstack_pause(schedp);
        if (p->context->rip != (uptr)forkret && 
            p->context->rip != (uptr)rcu_gc_worker)
        {
          mtrace_kstack_resume(proc);
        }
	mtrace_call_set(1, mycpu()->id);

	swtch(&mycpu()->scheduler, myproc()->context);
        mtrace_kstack_resume(schedp);
	mtrace_call_set(0, mycpu()->id);
	switchkvm();

	// Process is done running for now.
	// It should have changed its p->state before coming back.
	mycpu()->proc = schedp;
	if (p->state != RUNNABLE)
	  delrun(p);
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

    int now = ticks;
    if (now - mycpu()->last_rcu_gc_ticks > 100) {
      rcu_gc();
      mycpu()->last_rcu_gc_ticks = now;
    }

    if (idle[mycpu()->id]) {
      sti();
      hlt();
    }
  }
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
  struct vmap *m = myproc()->vmap;

  if(n < 0 && 0 - n <= myproc()->brk){
    myproc()->brk += n;
    return 0;
  }

  if(n < 0 || n > USERTOP || myproc()->brk + n > USERTOP)
    return -1;

  acquire(&m->lock);

  // find first unallocated address in brk..brk+n
  uptr newstart = myproc()->brk;
  u64 newn = n;
  rcu_begin_read();
  while(newn > 0){
    struct vma *e = vmap_lookup(m, newstart, 1);
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
    myproc()->brk += n;
    release(&m->lock);
    switchuvm(myproc());
    return 0;
  }

  // is there space for newstart..newstart+newn?
  if(vmap_lookup(m, newstart, newn) != 0){
    cprintf("growproc: not enough room in address space; brk %d n %d\n",
            myproc()->brk, n);
    return -1;
  }

  // would the newly allocated region abut the next-higher
  // vma? we can't allow that, since then a future sbrk()
  // would start to use the next region (e.g. the stack).
  if(vmap_lookup(m, PGROUNDUP(newstart+newn), 1) != 0){
    cprintf("growproc: would abut next vma; brk %d n %d\n",
            myproc()->brk, n);
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

  myproc()->brk += n;
  switchuvm(myproc());
  return 0;
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

  // XXX(sbw)
#if 0
  uint pc[10];
  if(p->state == SLEEPING){
    getcallerpcs((uint*)p->context->ebp+2, pc);
    for(int i=0; i<10 && pc[i] != 0; i++)
      cprintf(" %p", pc[i]);
  }
#endif
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

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(int flags)
{
  int i, pid;
  struct proc *np;
  int cow = 1;

  //  cprintf("%d: fork\n", proc->pid);

  // Allocate process.
  if((np = allocproc()) == 0)
    return -1;

  if(flags == 0) {
    // Copy process state from p.
    if((np->vmap = vmap_copy(myproc()->vmap, cow)) == 0){
      kfree(np->kstack);
      np->kstack = 0;
      np->state = UNUSED;
      if (ns_remove(nspid, KI(np->pid), np) == 0)
	panic("fork: ns_remove");
      rcu_delayed(np, kmfree);
      return -1;
    }
  } else {
    np->vmap = myproc()->vmap;
    __sync_fetch_and_add(&np->vmap->ref, 1);
  }

  np->brk = myproc()->brk;
  np->parent = myproc();
  *np->tf = *myproc()->tf;

  // Clear %eax so that fork returns 0 in the child.
  np->tf->rax = 0;

  for(i = 0; i < NOFILE; i++)
    if(myproc()->ofile[i])
      np->ofile[i] = filedup(myproc()->ofile[i]);
  np->cwd = idup(myproc()->cwd);
  pid = np->pid;
  safestrcpy(np->name, myproc()->name, sizeof(myproc()->name));
  acquire(&myproc()->lock);
  SLIST_INSERT_HEAD(&myproc()->childq, np, child_next);
  release(&myproc()->lock);

  acquire(&np->lock);
  addrun(np);
  np->state = RUNNABLE;
  release(&np->lock);

  migrate(np);

  //  cprintf("%d: fork done (pid %d)\n", myproc()->pid, pid);
  return pid;
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
    acquire(&myproc()->lock);
    SLIST_FOREACH_SAFE(p, &myproc()->childq, child_next, np) {
	havekids = 1;
	acquire(&p->lock);
	if(p->state == ZOMBIE){
	  release(&p->lock);	// noone else better be trying to lock p
	  pid = p->pid;
	  SLIST_REMOVE(&myproc()->childq, p, proc, child_next);
	  release(&myproc()->lock);
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
    if(!havekids || myproc()->killed){
      release(&myproc()->lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    cv_sleep(&myproc()->cv, &myproc()->lock);  

    release(&myproc()->lock);
  }
}
