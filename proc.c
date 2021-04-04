#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

#define DEFAULT_BUDGET 10
#define MAXPRIORITY 3
#define TICKS_TO_PROMOTE 50

/**
 * Logic to maintain the multilevel queue
 * Modified proc structure holds the next, prev values for the queue
 */
typedef struct {
  struct proc *head; 
  struct proc *tail;
} ProcessQueue;

struct {
  // allocate MAXPRIORITY queues
  ProcessQueue levels[MAXPRIORITY];
} priority_queue;

// debugging function
// prints the queue and does additional fault checking
// checks if the head incorrectly has a previous value, tail has a next etc.
void printqueue(ProcessQueue *queue) {
  struct proc *p = queue->head;
  if(p) {
    if(p->prev) {
      cprintf("prev of head %d is %d\n", p->pid, p->prev->pid);
      panic("fault in process queue(1.0)");
    }
    int encountered_tail = 0;
    while(p) {
      cprintf(" -> %d", p->pid);
      if(p == queue->tail) {
        if(p->next) {
          panic("fault in process queue(1.1)");
        }
        encountered_tail = 1;
      }
      if(p->next) {
        if(p->next->prev != p) {
          panic("fault in process queue(1.2)");
        }
      }
      p = p->next;
    }
    if(queue->tail && !encountered_tail) {
      panic("fault in process queue(1.3)");
    }
  }
  cprintf("\n");
}
/** pushes a process onto the queue of it's priority */
void push(struct proc *p) {
  if(p->priority < 0) {
    panic("fault in process queue(2.0)");
  }
  if(p->inqueue) {
    panic("fault in process queue(2.1)");
  }
  ProcessQueue *queue = priority_queue.levels + p->priority;
  if(!queue->head) { // 0 items
    queue->head = p;
    p->prev = 0;
  } else {
    if(queue->tail) { // 1+ items
      queue->tail->next = p;
      p->prev = queue->tail;
    } else { // 1 item
      if(queue->head->next) {
        panic("fault in process queue(2.2)");
      }
      queue->head->next = p;
      p->prev = queue->head;
    }
    queue->tail = p;
  }
  p->next = 0;
  p->inqueue = 1;
  // additional checks, commented for efficiency
  /*if(queue->head && queue->head->prev) {
    panic("fault in process queue(2.3)");
  }
  if(queue->tail && queue->tail->next) {
    panic("fault in process queue(2.4)");
  }*/
}
// remove a process from its queue
void remove(struct proc *p) {
  if(p->priority < 0) {
    panic("fault in process queue(3.0)");
  }
  int priority = p->priority;
  ProcessQueue *queue = priority_queue.levels + priority;
  if(p == queue->head) {
    queue->head = p->next;
    if(p->next) {
      p->next->prev = 0;
    }
  } else if(p == queue->tail) {
    queue->tail = p->prev;
    queue->tail->next = 0;
  } else {
    p->prev->next = p->next;
    p->next->prev = p->prev;
  }
  if(queue->head == queue->tail) {
    queue->tail = 0;
  }
  p->inqueue = 0;
  p->next = 0;
  p->prev = 0;
}
// set the process as runnable
// adds it to the required queue if necessary
void setrunnable(struct proc *p) {
  p->state = RUNNABLE;
  if(p->priority < 0) {
    p->priority = MAXPRIORITY-1;
  }
  if(!p->inqueue) {
    push(p);
    //cprintf("added %d to queue (%d): ", p->pid, p->priority); printqueue(priority_queue.levels + p->priority);
  }
}
// small wrapper function to get ticks
uint getticks() {
  return ticks;
}

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
  unsigned int promote_at_time;
} ptable;

static struct proc *initproc;

int nextpid = 1;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

void
pinit(void)
{
  initlock(&ptable.lock, "ptable");
}

// Must be called with interrupts disabled
int
cpuid() {
  return mycpu()-cpus;
}

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
struct cpu*
mycpu(void)
{
  int apicid, i;
  
  if(readeflags()&FL_IF)
    panic("mycpu called with interrupts enabled\n");
  
  apicid = lapicid();
  // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // a reverse map, or reserve a register to store &cpus[i].
  for (i = 0; i < ncpu; ++i) {
    if (cpus[i].apicid == apicid)
      return &cpus[i];
  }
  panic("unknown apicid\n");
}

// Disable interrupts so that we are not rescheduled
// while reading proc from the cpu structure
struct proc*
myproc(void) {
  struct cpu *c;
  struct proc *p;
  pushcli();
  c = mycpu();
  p = c->proc;
  popcli();
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
  char *sp;

  acquire(&ptable.lock);
  int i = 0;
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
    if(p->state == UNUSED)
      goto found;
    i+= 1;
  }
    
  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;
  p->budget = DEFAULT_BUDGET;
  p->priority = MAXPRIORITY-1;
  p->inqueue = 0;
  p->next = 0;
  p->prev = 0;
  //push(p, priority_queue.levels + MAXPRIORITY-1); // add to high priority queue

  release(&ptable.lock);

  // Allocate kernel stack.
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
void updatepromotiontime(void) {
  ptable.promote_at_time = getticks() + TICKS_TO_PROMOTE;
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
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
  p->sz = PGSIZE;
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

  // this assignment to p->state lets other cores
  // run this process. the acquire forces the above
  // writes to be visible, and the lock is also needed
  // because the assignment might not be atomic.
  acquire(&ptable.lock);

  updatepromotiontime();
  setrunnable(p);

  release(&ptable.lock);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
  uint sz;
  struct proc *curproc = myproc();

  sz = curproc->sz;
  if(n > 0){
    if((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  } else if(n < 0){
    if((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  curproc->sz = sz;
  switchuvm(curproc);
  return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(void)
{
  int i, pid;
  struct proc *np;
  struct proc *curproc = myproc();

  // Allocate process.
  if((np = allocproc()) == 0){
    return -1;
  }

  // Copy process state from proc.
  if((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0){
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }
  np->sz = curproc->sz;
  np->parent = curproc;
  *np->tf = *curproc->tf;

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;

  for(i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
  np->cwd = idup(curproc->cwd);

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));

  pid = np->pid;

  acquire(&ptable.lock);

  setrunnable(np);

  release(&ptable.lock);

  return pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void)
{
  struct proc *curproc = myproc();
  struct proc *p;
  int fd;

  if(curproc == initproc)
    panic("init exiting");

  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
    if(curproc->ofile[fd]){
      fileclose(curproc->ofile[fd]);
      curproc->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(curproc->cwd);
  end_op();
  curproc->cwd = 0;

  acquire(&ptable.lock);

  // Parent might be sleeping in wait().
  wakeup1(curproc->parent);

  // Pass abandoned children to init.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == curproc){
      p->parent = initproc;
      if(p->state == ZOMBIE)
        wakeup1(initproc);
    }
  }

  // Jump into the scheduler, never to return.
  curproc->state = ZOMBIE;
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(void)
{
  struct proc *p;
  int havekids, pid;
  struct proc *curproc = myproc();
  
  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != curproc)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || curproc->killed){
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock);  //DOC: wait-sleep
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
  struct proc *p, *next, *tail;
  struct cpu *c = mycpu();
  int idx = 0, newidx;
  uint startticks;
  c->proc = 0;
  
  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);

    if(getticks() > ptable.promote_at_time) {
      // promote all processes
      for(idx = MAXPRIORITY-2;idx >= 0;idx--) {
        p = priority_queue.levels[idx].head;
        while(p) {
          next = p->next;
          if(p->state == RUNNABLE) {
            remove(p);
            p->priority = p->priority+1;
            push(p);
          }
          p = next;
        }
      }
      updatepromotiontime(); // next promotion time
    }
    
    for(idx = MAXPRIORITY-1;idx >= 0;idx--) {
      p = priority_queue.levels[idx].head;
      tail = priority_queue.levels[idx].tail;
      while(p) {
        next = p->next; // cache the next process
        if(p->state != RUNNABLE) {
          // ignore if not runnable
        } else {
          //cprintf("start exec of pid=%d: ", p->pid);
          //printqueue(priority_queue.levels + idx);
          startticks = getticks(); // start ticks
          // Switch to chosen process.  It is the process's job
          // to release ptable.lock and then reacquire it
          // before jumping back to us.
          c->proc = p;
          switchuvm(p);
          p->state = RUNNING;

          swtch(&(c->scheduler), p->context);
          switchkvm();
          // Process is done running for now.
          // It should have changed its p->state before coming back.

          c->proc = 0;
          // update the budget
          p->budget -= (getticks() - startticks);
          //cprintf("pid %d budget %d\n", p->pid, p->budget);
          //cprintf("removing %d from (%d): ", p->pid, idx); printqueue(priority_queue.levels + idx);
          remove(p); // remove the process from the queue
          if(p->budget <= 0) { // if process has used it's budget
            // reset budget
            p->budget = DEFAULT_BUDGET;
            // demote process
            newidx = (idx > 0 ? idx - 1 : 0);
          } else newidx = idx; // push back on same queue if budget left

          p->priority = newidx;
          if(p->state == RUNNABLE) {
            setrunnable(p); // run the process again if runnable
          }
          // if the process isn't runnable -- then some other mechanism will add it to the queue
          //cprintf("added %d to (%d): ", p->pid, newidx); printqueue(priority_queue.levels + newidx);
        }
        if(p == tail) {
          break;
        }
        p = next;
      }
    }
    release(&ptable.lock);
  }
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
void
sched(void)
{
  int intena;
  struct proc *p = myproc();

  if(!holding(&ptable.lock))
    panic("sched ptable.lock");
  if(mycpu()->ncli != 1)
    panic("sched locks");
  if(p->state == RUNNING)
    panic("sched running");
  if(readeflags()&FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
  swtch(&p->context, mycpu()->scheduler);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
  acquire(&ptable.lock);  //DOC: yieldlock
  setrunnable(myproc());
  sched();
  release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void)
{
  static int first = 1;
  // Still holding ptable.lock from scheduler.
  release(&ptable.lock);

  if (first) {
    // Some initialization functions must be run in the context
    // of a regular process (e.g., they call sleep), and thus cannot
    // be run from main().
    first = 0;
    iinit(ROOTDEV);
    initlog(ROOTDEV);
  }

  // Return to "caller", actually trapret (see allocproc).
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

  // Must acquire ptable.lock in order to
  // change p->state and then call sched.
  // Once we hold ptable.lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with ptable.lock locked),
  // so it's okay to release lk.
  if(lk != &ptable.lock){  //DOC: sleeplock0
    acquire(&ptable.lock);  //DOC: sleeplock1
    release(lk);
  }
  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  if(lk != &ptable.lock){  //DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
  }
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan)
      setrunnable(p);
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
  struct proc *p;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        setrunnable(p);
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}
struct proc *find_by_pid(int pid) {
  struct proc *p;
  for(p = ptable.proc;p < ptable.proc + NPROC;p++) {
    if(p->pid == pid) {
      if(p->state == UNUSED || p->state == ZOMBIE) {
        break;
      }
      return p;
    }
  }
  return 0;
}

int setpriority(int pid, int priority) {
  if(priority < 0 || priority >= MAXPRIORITY) {
    return -1;
  }
  acquire(&ptable.lock); // acquire lock
  int result = -1;
  struct proc *p = find_by_pid(pid); // get the process
  if(p) {
    if(p->inqueue) { // remove, if present in some queue
      remove(p);
    }
    p->priority = priority; // set new priority
    p->budget = DEFAULT_BUDGET; // & reset time budget
    push(p); // update
    result = 0;
  }
  release(&ptable.lock); 
  return result;
}
int getpriority(int pid) {
  int result = -1;
  acquire(&ptable.lock);

  struct proc *p = find_by_pid(pid);
  if(p) result = p->priority;

  release(&ptable.lock); 
  return result;
}

//PAGEBREAK: 36
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
  int i;
  struct proc *p;
  char *state;
  uint pc[10];

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
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
}
