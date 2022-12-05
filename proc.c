#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"
#include "rand.h"

#define STARTING_PRIORITY 2
#define ZERO_TO_ONE 20
#define ONE_TO_TWO 20

// Implementando a fila
struct Fila
{
  int primeiro, ultimo, tamanho;
  struct proc *fila[NPROC];
};

void instanciaFila(struct Fila *fila)
{
  fila->tamanho = 0;
  int i;
  for (i = 0; i < NPROC; i++)
  {
    fila->fila[i] = 0;
  }
}

int filaCheia(struct Fila *fila)
{
  return (fila->tamanho == NPROC);
}

int filaVazia(struct Fila *fila)
{
  return (fila->tamanho == 0);
}

void adicionaFila(struct Fila *fila, struct proc *processo)
{
  if (filaCheia(fila))
    return;
  int i = fila->tamanho;
  fila->fila[i] = processo;
  fila->tamanho++;
}

void removeFila(struct Fila *fila, struct proc *processo)
{
  if (processo == 0)
    return;
  if (filaVazia(fila))
    return;

  int i = 0;
  int flagProcesso = 0;
  while (i < NPROC)
  {
    if (fila->fila[i] && fila->fila[i]->pid == processo->pid)
    {
      flagProcesso = 1;
      break;
    }
    i++;
  }

  if (flagProcesso)
  {
    while (i < NPROC - 1)
    {
      fila->fila[i] = fila->fila[i + 1];
      i++;
    }
    fila->fila[fila->tamanho - 1] = 0;
    fila->tamanho--;
  }
}

struct proc *getPrimeiroFila(struct Fila *fila)
{
  int i;
  for (i = 0; i < NPROC; i++)
  {
    if (fila->fila[i] && fila->fila[i]->state == RUNNABLE)
      return fila->fila[i];
  }
  return 0;
}

// Instanciando as 3 filas de prioridade
struct Fila fila0;
struct Fila fila1;
struct Fila fila2;

void iniciaFilas(void)
{
  instanciaFila(&fila0);
  instanciaFila(&fila1);
  instanciaFila(&fila2);
}
struct
{
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

static struct proc *initproc;

int nextpid = 1;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

void pinit(void)
{
  initlock(&ptable.lock, "ptable");
  iniciaFilas();
}

// Must be called with interrupts disabled
int cpuid()
{
  return mycpu() - cpus;
}

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
struct cpu *
mycpu(void)
{
  int apicid, i;

  if (readeflags() & FL_IF)
    panic("mycpu called with interrupts enabled\n");

  apicid = lapicid();
  // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // a reverse map, or reserve a register to store &cpus[i].
  for (i = 0; i < ncpu; ++i)
  {
    if (cpus[i].apicid == apicid)
      return &cpus[i];
  }
  panic("unknown apicid\n");
}

// Disable interrupts so that we are not rescheduled
// while reading proc from the cpu structure
struct proc *
myproc(void)
{
  struct cpu *c;
  struct proc *p;
  pushcli();
  c = mycpu();
  p = c->proc;
  popcli();
  return p;
}

// PAGEBREAK: 32
//  Look in the process table for an UNUSED proc.
//  If found, change state to EMBRYO and initialize
//  state required to run in the kernel.
//  Otherwise return 0.
static struct proc *
allocproc(void)
{
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);

  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if (p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;
  p->tickets = 10;
  p->tipoScheduler = 0;

  release(&ptable.lock);

  // Allocate kernel stack.
  if ((p->kstack = kalloc()) == 0)
  {
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;

  // Leave room for trap frame.
  sp -= sizeof *p->tf;
  p->tf = (struct trapframe *)sp;

  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint *)sp = (uint)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context *)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;
  p->prioridade = STARTING_PRIORITY;
  adicionaFila(&fila2, p);

  return p;
}

// PAGEBREAK: 32
//  Set up first user process.
void userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];

  p = allocproc();

  initproc = p;
  if ((p->pgdir = setupkvm()) == 0)
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
  p->tf->eip = 0; // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  // this assignment to p->state lets other cores
  // run this process. the acquire forces the above
  // writes to be visible, and the lock is also needed
  // because the assignment might not be atomic.
  acquire(&ptable.lock);

  p->state = RUNNABLE;

  release(&ptable.lock);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int growproc(int n)
{
  uint sz;
  struct proc *curproc = myproc();

  sz = curproc->sz;
  if (n > 0)
  {
    if ((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  else if (n < 0)
  {
    if ((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  curproc->sz = sz;
  switchuvm(curproc);
  return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int fork(void)
{
  int i, pid;
  struct proc *np;
  struct proc *curproc = myproc();

  // Allocate process.
  if ((np = allocproc()) == 0)
  {
    return -1;
  }

  // Copy process state from proc.
  if ((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0)
  {
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
      np->ofile[i] = filedup(curproc->ofile[i]);
  np->cwd = idup(curproc->cwd);

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));

  pid = np->pid;

  acquire(&ptable.lock);

  np->state = RUNNABLE;

  release(&ptable.lock);

  return pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void exit(void)
{
  struct proc *curproc = myproc();
  struct proc *p;
  int fd;

  if (curproc == initproc)
    panic("init exiting");

  // Close all open files.
  for (fd = 0; fd < NOFILE; fd++)
  {
    if (curproc->ofile[fd])
    {
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
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if (p->parent == curproc)
    {
      p->parent = initproc;
      if (p->state == ZOMBIE)
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
int wait(void)
{
  struct proc *p;
  int havekids, pid;
  struct proc *curproc = myproc();

  acquire(&ptable.lock);
  for (;;)
  {
    // Scan through table looking for exited children.
    havekids = 0;
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    {
      if (p->parent != curproc)
        continue;
      havekids = 1;
      if (p->state == ZOMBIE)
      {
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
        // reseta estados de teste
        p->ctime = 0;
        p->retime = 0;
        p->rutime = 0;
        p->stime = 0;
        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if (!havekids || curproc->killed)
    {
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock); // DOC: wait-sleep
  }
}

// PAGEBREAK: 42
//  Per-CPU process scheduler.
//  Each CPU calls scheduler() after setting itself up.
//  Scheduler never returns.  It loops, doing:
//   - choose a process to run
//   - swtch to start running that process
//   - eventually that process transfers control
//       via swtch back to the scheduler.
void scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();
  c->proc = 0;
  int foundproc = 1;
  int count = 0;
  long golden_ticket = 0;
  int total_no_tickets = 0;

  for (;;)
  {
    if (c->proc->tipoScheduler == 1)
    {
      // Enable interrupts on this processor.
      sti();

      // Loop over process table looking for process to run.
      acquire(&ptable.lock);
      p = 0;
      if (!filaVazia(&fila2))
        p = getPrimeiroFila(&fila2);
      if (p == 0)
      {
        if (!filaVazia(&fila0))
          p = getPrimeiroFila(&fila1);

        if (p == 0)
          if (!filaVazia(&fila0))
            p = getPrimeiroFila(&fila0);
      }
      if (p)
      {
        // colocando o processo no fim da sua fila.
        if (p->prioridade == 0)
        {
          removeFila(&fila0, p);
          adicionaFila(&fila0, p);
        }
        else if (p->prioridade == 1)
        {
          removeFila(&fila1, p);
          adicionaFila(&fila1, p);
        }
        else
        {
          removeFila(&fila2, p);
          adicionaFila(&fila2, p);
        }
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
      }
      release(&ptable.lock);
    } else if (c->proc->tipoScheduler == 0){
      // Enable interrupts on this processor.
    sti();

    if (!foundproc) hlt();
    foundproc = 0;

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);

    //resetting the variables to make scheduler start from the beginning of the process queue
    golden_ticket = 0;
    count = 0;
    total_no_tickets = 0;
    
    //calculate Total number of tickets for runnable processes  
    
    total_no_tickets = lottery_Total();

    //pick a random ticket from total available tickets
    golden_ticket = random_at_most(total_no_tickets);
 
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;

      //find the process which holds the lottery winning ticket 
      if ((count + p->tickets) < golden_ticket){
        count += p->tickets;
        continue;
      }

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      foundproc = 1;
      c->proc = p;
      switchuvm(p);
      p->state = RUNNING;
      swtch(c->scheduler, c->proc->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
      break;
    }
    release(&ptable.lock);
    }
  }
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
void sched(void)
{
  int intena;
  struct proc *p = myproc();

  if (!holding(&ptable.lock))
    panic("sched ptable.lock");
  if (mycpu()->ncli != 1)
    panic("sched locks");
  if (p->state == RUNNING)
    panic("sched running");
  if (readeflags() & FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
  swtch(&p->context, mycpu()->scheduler);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
void yield(void)
{
  acquire(&ptable.lock); // DOC: yieldlock
  myproc()->state = RUNNABLE;
  sched();
  release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void forkret(void)
{
  static int first = 1;
  // Still holding ptable.lock from scheduler.
  release(&ptable.lock);

  if (first)
  {
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
void sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();

  if (p == 0)
    panic("sleep");

  if (lk == 0)
    panic("sleep without lk");

  // Must acquire ptable.lock in order to
  // change p->state and then call sched.
  // Once we hold ptable.lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with ptable.lock locked),
  // so it's okay to release lk.
  if (lk != &ptable.lock)
  {                        // DOC: sleeplock0
    acquire(&ptable.lock); // DOC: sleeplock1
    release(lk);
  }
  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  if (lk != &ptable.lock)
  { // DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
  }
}

// PAGEBREAK!
//  Wake up all processes sleeping on chan.
//  The ptable lock must be held.
static void
wakeup1(void *chan)
{
  struct proc *p;

  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if (p->state == SLEEPING && p->chan == chan)
      p->state = RUNNABLE;
}

// Wake up all processes sleeping on chan.
void wakeup(void *chan)
{
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int kill(int pid)
{
  struct proc *p;

  acquire(&ptable.lock);
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if (p->pid == pid)
    {
      p->killed = 1;
      // Wake process from sleep if necessary.
      if (p->state == SLEEPING)
        p->state = RUNNABLE;
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}

// PAGEBREAK: 36
//  Print a process listing to console.  For debugging.
//  Runs when user types ^P on console.
//  No lock to avoid wedging a stuck machine further.
void procdump(void)
{
  static char *states[] = {
      [UNUSED] "unused",
      [EMBRYO] "embryo",
      [SLEEPING] "sleep ",
      [RUNNABLE] "runble",
      [RUNNING] "run   ",
      [ZOMBIE] "zombie"};
  int i;
  struct proc *p;
  char *state;
  uint pc[10];

  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if (p->state == UNUSED)
      continue;
    if (p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    cprintf("%d %s %s", p->pid, state, p->name);
    if (p->state == SLEEPING)
    {
      getcallerpcs((uint *)p->context->ebp + 2, pc);
      for (i = 0; i < 10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
  }
}

int set_prio(int _prioridade)
{
  if (_prioridade < 0 || _prioridade > 2)
    return -1;

  int prioridadeAnterior = myproc()->prioridade;

  acquire(&ptable.lock);
  myproc()->prioridade = _prioridade;
  release(&ptable.lock);

  if (_prioridade == 2)
    adicionaFila(&fila2, myproc());
  else if (_prioridade == 1)
    adicionaFila(&fila1, myproc());
  else
    adicionaFila(&fila0, myproc());

  if (prioridadeAnterior == 2)
    removeFila(&fila2, myproc());
  else if (prioridadeAnterior == 1)
    removeFila(&fila1, myproc());
  else
    removeFila(&fila0, myproc());

  return 0;
}

int wait2(int *retime, int *rutime, int *stime)
{
  struct proc *processo;
  int temFilhos, pid;
  struct proc *processoAtual = myproc();
  acquire(&ptable.lock);
  for (;;)
  {
    temFilhos = 0;
    for (processo = ptable.proc; processo < &ptable.proc[NPROC]; processo++)
    {
      if (processo->parent != processoAtual)
        continue;
      temFilhos = 1;
      if (processo->state == ZOMBIE)
      {
        *retime = processo->retime;
        *rutime = processo->rutime;
        *stime = processo->stime;
        pid = processo->pid;
        kfree(processo->kstack);
        processo->kstack = 0;
        freevm(processo->pgdir);
        processo->pid = 0;
        processo->parent = 0;
        processo->name[0] = 0;
        processo->killed = 0;
        processo->state = UNUSED;
        processo->ctime = 0;
        processo->retime = 0;
        processo->rutime = 0;
        processo->stime = 0;
        release(&ptable.lock);
        return pid;
      }
    }
    if (!temFilhos || processoAtual->killed)
    {
      release(&ptable.lock);
      return -1;
    }
    sleep(processoAtual, &ptable.lock);
  }
}

void update_stats(void)
{
  struct proc *processo;
  acquire(&ptable.lock);

  for (processo = ptable.proc; processo < &ptable.proc[NPROC]; processo++)
  {
    if (processo->state == SLEEPING)
      processo->stime++;
    else if (processo->state == RUNNABLE)
      processo->retime++;
    else if (processo->state == RUNNING)
      processo->rutime++;
  }
  release(&ptable.lock);
}

void aging(void)
{
  struct proc *processo;
  acquire(&ptable.lock);

  for (processo = ptable.proc; processo < &ptable.proc[NPROC]; processo++)
  {
    if (processo->prioridade == 0)
    {
      if (processo->retime >= ZERO_TO_ONE)
      {
        adicionaFila(&fila1, processo);
        removeFila(&fila0, processo);
        processo->prioridade++;
      }
    }
    else if (processo->prioridade == 1)
    {
      if (processo->retime >= ONE_TO_TWO)
      {
        adicionaFila(&fila2, processo);
        removeFila(&fila1, processo);
        processo->prioridade++;
      }
    }
  }
  release(&ptable.lock);
}

int user_yield(void)
{
  yield();
  return 0;
}

int set_tickets(int tickets)
{
  struct proc *p;
  p = myproc();
  p->tickets = tickets;
  return 0;
}

int set_scheduler(int scheduler)
{
  struct proc *p;
  p = myproc();
  p->tipoScheduler = scheduler;
  return 0;
}