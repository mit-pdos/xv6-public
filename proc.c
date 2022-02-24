#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"
#include "wstatus.h"
#include "pid_ns.h"
#include "namespace.h"
#include "cpu_account.h"
#include "obj_log.h"

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

/*Return the process id inside the given namespace, else returns zero*/
int get_pid_for_ns(struct proc* proc, struct pid_ns* pid_ns) {
  for (int i = 0; i < MAX_PID_NS_DEPTH; i++) {
    if (proc->pids[i].pid_ns == pid_ns) {
      return proc->pids[i].pid;
    }
  }
  return 0;
}

int proc_pid(struct proc* proc) {
    return get_pid_for_ns(proc, myproc()->nsproxy->pid_ns);
}

static struct proc *initproc;

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

// Halt the processor.
static void hlt()
{
    asm("hlt");
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

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->killed = 0; // Prevents process allocating with killed=1 flag. This behavior was exhibited as a bug and this prevents it.
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

  // Set cgroup to none.
  p->cgroup = 0;

  // Set cpu information.
  p->cpu_account_frame = 0;
  p->cpu_time = 0;
  p->cpu_period_time = 0;
  p->cpu_percent = 0;

  return p;
}

//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];

  // Initialize the root cgroup.
  cgroup_initialize(cgroup_root(), 0, 0);

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
  p->cwd = initprocessroot(&p->cwdmount);
  safestrcpy(p->cwdp, "/", sizeof(p->cwdp));
  p->nsproxy = emptynsproxy();

  p->ns_pid = pid_ns_next_pid(p->nsproxy->pid_ns);

  p->pids[0].pid = p->ns_pid;
  p->pids[0].pid_ns = p->nsproxy->pid_ns;

  // this assignment to p->state lets other cores
  // run this process. the acquire forces the above
  // writes to be visible, and the lock is also needed
  // because the assignment might not be atomic.
  acquire(&ptable.lock);

  // Associate with the root cgroup, can discard return value because
  // this fails only if there is no room.
  cgroup_insert(cgroup_root(), p);

  // Set state to runnable.
  p->state = RUNNABLE;

  release(&ptable.lock);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
  uint sz;
  struct proc* curproc = myproc();

  // In case trying to grow process's memory over memory limit, and
  // given memory controller is enabled, return failure
  if (n > 0) {
    if (curproc->cgroup->mem_controller_enabled &&
      (curproc->cgroup->current_mem + n) > curproc->cgroup->max_mem)
      return -1;
  }

  sz = curproc->sz;
  if (n > 0) {
    if ((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  else if (n < 0) {
    if ((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  curproc->sz = sz;

  // Update memory usage in cgroup and its ancestors
  struct cgroup* cgroup = curproc->cgroup;
  do {
    cgroup->current_mem += n;
  } while ((cgroup = cgroup->parent));

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

  // Check if the current process has a child pid namespace and if its pid1 was killed.
  if (curproc->child_pid_ns && curproc->child_pid_ns->pid1_ns_killed) {
    return -1;
  }

  // In case trying to fork new process and the cgroup reached its process limit,
  // given pid controller is enabled, return failure
  if ( curproc->cgroup->pid_controller_enabled &&
       (curproc->cgroup->num_of_procs + 1) > curproc->cgroup->max_num_of_procs )
           return -1;

  // In case trying to fork a new process and the cgroup reached its memory limit,
  // given memory controller is enabled, return failure
  if (curproc->cgroup->mem_controller_enabled &&
    (curproc->cgroup->current_mem + curproc->sz) > curproc->cgroup->max_mem)
    return -1;

  // Allocate process.
  if ((np = allocproc()) == 0) {
    return -1;
  }

  // Copy process state from proc.
  if ((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0) {
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

  for (i = 0; i < NOFILE; i++)
    if (curproc->ofile[i])
      np->ofile[i] = vfs_filedup(curproc->ofile[i]);
  np->cwd = curproc->cwd->i_op.idup(curproc->cwd);
  safestrcpy(np->cwdp, curproc->cwdp, sizeof(curproc->cwdp));
  np->cwdmount = mntdup(curproc->cwdmount);

  struct pid_ns* cur = curproc->child_pid_ns;
  if (cur) {
    np->nsproxy = namespace_replace_pid_ns(curproc->nsproxy, cur);
  } else {
    np->nsproxy = namespacedup(curproc->nsproxy);
    cur = np->nsproxy->pid_ns;
  }

  // for each pid_ns get me a pid
  i = 0;
  while (cur) {
    if (i >= MAX_PID_NS_DEPTH) {
      panic("too many danif!");
    }

    np->pids[i].pid = pid_ns_next_pid(cur);
    np->pids[i].pid_ns = cur;
    i++;
    cur = cur->parent;
  }

  np->ns_pid = np->pids[0].pid;

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));

  pid = get_pid_for_ns(np, curproc->nsproxy->pid_ns);

  acquire(&ptable.lock);

  // Associate the new process with the current process cgroup.
  // can discard return value because this fails only if there is no room
  // which is checked earlier by allocproc.
  cgroup_insert(curproc->cgroup, np);

  // Set new process to runnable.
  np->state = RUNNABLE;

  release(&ptable.lock);

  return pid;
}
/*Kill the given process p, and set its parent to given process reaper*/
void kill_proc(struct proc* p, struct proc* reaper) {
   p->killed = 1;
   if (p->state == SLEEPING)
    p->state = RUNNABLE;
   p->parent = reaper;
}

/*Kill all the processes inside the namespace of a given process, called parent
reaper in this case becomes their parent process*/
void kill_all_pid_ns(struct proc* parent, struct proc* reaper, struct pid_ns* curpidns) {
  struct proc *p;
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    /*If the process is not the parent, and it is in the same namespace as parent, kill it*/
    if(p != parent && p->nsproxy->pid_ns == curpidns) {
      kill_proc(p, reaper);
    }
  }
}

/*Return the process whose pid=1 inside the given namespace*/
struct proc* get_pid1_for_ns(struct pid_ns* pid_ns) {
  for (struct proc* p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
    if(get_pid_for_ns(p, pid_ns) == 1){
      return p;
    }
  }
  return 0;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(int status)
{
  struct proc *curproc = myproc();
  struct proc *p;
  struct pid_ns *curpidns;
  int fd;

  curproc->status = status;

  if(curproc == initproc)
    panic("init exiting");

  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
    if(curproc->ofile[fd]){
      vfs_fileclose(curproc->ofile[fd]);
      curproc->ofile[fd] = 0;
    }
  }

  begin_op();
  curproc->cwd->i_op.iput(curproc->cwd);
  end_op();

  mntput(curproc->cwdmount);
  curproc->cwdmount = 0;
  *curproc->cwdp = 0;
  curproc->cwd = 0;

  struct proc* procpid1 = 0;
  // Find process with pid 1 within namespace
  procpid1 = get_pid1_for_ns(curproc->nsproxy->pid_ns);

  // Here we could not find process with pid 1 inside the namespace, and pid 1 wasn't marked as killed
  if (procpid1 == 0 && curproc->nsproxy->pid_ns->pid1_ns_killed == 0)
    panic("couldn't find pid 1");
  curpidns = curproc->nsproxy->pid_ns;

  namespaceput(curproc->nsproxy);

  acquire(&ptable.lock);
  // Parent might be sleeping in wait().
  wakeup1(curproc->parent);

  // If the current process holds pid 1 within its namespace, mark all child processes as killed
  if (curproc->ns_pid == 1) {
    curpidns->pid1_ns_killed = 1; // Mark pid 1 process was killed

    kill_all_pid_ns(curproc, curproc->parent, curpidns); // Documentation for this command see above

  } else { // The current process does not hold pid 1 within its namespace

    // Pass the child processes of the current process to pid 1 process within the namespace
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent == curproc){
        p->parent = procpid1;
        if(p->state == ZOMBIE) {
          wakeup1(initproc);
        }
      }
    }
  }


  if (curproc->child_pid_ns)
    pid_ns_put(curproc->child_pid_ns);

  // Remove the process cgroup.
  cgroup_erase(curproc->cgroup, curproc);

  // Jump into the scheduler, never to return.
  curproc->state = ZOMBIE;
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(int *wstatus)
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
        pid = get_pid_for_ns(p, curproc->nsproxy->pid_ns);
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->ns_pid = 0;
        memset(p->pids, 0, sizeof(p->pids));
        p->child_pid_ns = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
        if (wstatus != 0)
         *wstatus = W_STOPCODE(p->status);
        p->status = 0;

        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if(havekids == 0 || curproc->killed){
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
  struct cpu_account cpu;
  struct proc *p = 0;
  struct cpu *c = mycpu();
  c->proc = 0;

  // Initialize the cpu account.
  cpu_account_initialize(&cpu);

  for(;;){
    // The amount of processes that have been scheduled in this run.
    unsigned int scheduled = 0;

    // Enable interrupts on this processor.
    sti();

    // Take the ptable lock.
    acquire(&ptable.lock);

    // Start schedule.
    cpu_account_schedule_start(&cpu);

    // Loop over process table looking for process to run.
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
      // Update proc information.
      cpu_account_schedule_proc_update(&cpu, p);

      // If process not runnable, continue.
      if (p->state != RUNNABLE) {
        continue;
      }

      // Cpu set controller and freezer are only defined on runnable processes which are not killed.
      if (p->killed == 0) {
          // If the cpu set controller enabled, and the cpu doesn't match the one that is supposed to run the process
          // then don't let the process run on this cpu.
          if (p->cgroup->set_controller_enabled && p->cgroup->cpu_to_use != c->apicid) {
              continue;
          }

          // If the group is frozen, don't schedule it.
          if (p->cgroup->is_frozen == 1) {
              continue;
          }
      }

      // Decide whether to schedule process.
      if (!cpu_account_schedule_process_decision(&cpu, p)) {
        continue;
      }

      // Increment scheduled.
      ++scheduled;

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      c->proc = p;

      // Switch to user page table.
      switchuvm(p);

      // Change process state to running.
      p->state = RUNNING;

      // Before process schedule callback.
      cpu_account_before_process_schedule(&cpu, p);

      // Switch to process.
      swtch(&(c->scheduler), p->context);

      // After process schedule callback.
      cpu_account_after_process_schedule(&cpu, p);

      // Switch to kernel page table.
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
    }
    release(&ptable.lock);

    // If a process was scheduled, continue.
    if (scheduled) {
      continue;
    }

    // No processes were scheduled, go to sleep.
    cpu_account_before_hlt(&cpu);
    hlt();
    cpu_account_after_hlt(&cpu);
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
  myproc()->state = RUNNABLE;
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
    init_objfs_log();
    mntinit(); // initialize mounts
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
      p->state = RUNNABLE;
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
  struct pid_ns* pid_ns = myproc()->nsproxy->pid_ns;
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(get_pid_for_ns(p, pid_ns) == pid){
       kill_proc(p, p->parent);
       release(&ptable.lock);
       return 0;
    }
  }
  release(&ptable.lock);
  return -1;
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
    cprintf("%d %s %s", get_pid_for_ns(p, initproc->nsproxy->pid_ns), state, p->name);
    if(p->state == SLEEPING){
      getcallerpcs((uint*)p->context->ebp+2, pc);
      for(i=0; i<10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
  }
}

void proc_lock()
{
    acquire(&ptable.lock);
}

void proc_unlock()
{
    release(&ptable.lock);
}

/*
* Move a process from one cgroup to another.
* Acquires lock, checks if process is alive, calls cgroup_insert, and releases lock.
*/
int
cgroup_move_proc(struct cgroup * cgroup, int pid)
{
    struct proc *p;

    acquire(&ptable.lock);

    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
        if(proc_pid(p) == pid)
            if(p->state == SLEEPING || p->state == RUNNABLE || p->state == RUNNING)
                if(unsafe_cgroup_insert(cgroup, p) == 0){
                    release(&ptable.lock);
                    return 0;
                }
    }
    release(&ptable.lock);
    return -1;
}

struct cgroup *proc_get_cgroup(void)
{
  struct cgroup *cg = 0;
  struct proc *proc = myproc();
  if (proc)
     cg = proc->cgroup;
  return cg;
}
