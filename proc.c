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
  cprintf("newproc pid=%d return to %x:%x tf-%p\n", np->pid, np->tf->tf_cs, np->tf->tf_eip, np->tf);

  // set up new jmpbuf to start executing at trapret with esp pointing at tf
  memset(&np->jmpbuf, 0, sizeof np->jmpbuf);
  np->jmpbuf.jb_eip = (unsigned) trapret;
  np->jmpbuf.jb_esp = (unsigned) np->tf - 4;  // -4 for the %eip that isn't actually there

  // copy file descriptors
  for(fd = 0; fd < NOFILE; fd++){
    np->fds[fd] = op->fds[fd];
    if(np->fds[fd])
      fd_reference(np->fds[fd]);
  }

  cprintf("newproc %x\n", np);

  return np;
}

void
scheduler(void)
{
  struct proc *op, *np;
  int i;

  cprintf("start scheduler on cpu %d jmpbuf %p\n", cpu(), &cpus[cpu()].jmpbuf);
  cpus[cpu()].lastproc = &proc[0];

  setjmp(&cpus[cpu()].jmpbuf);

  op = curproc[cpu()];
  if(op){
    if(op->newstate <= 0 || op->newstate > ZOMBIE)
      panic("scheduler");
    op->state = op->newstate;
    op->newstate = -1;
  }

  // find a runnable process and switch to it
  curproc[cpu()] = 0;
  np = cpus[cpu()].lastproc + 1;
  while(1){
    acquire(&proc_table_lock);
    for(i = 0; i < NPROC; i++){
      if(np >= &proc[NPROC])
        np = &proc[0];
      if(np->state == RUNNABLE)
        break;
      np++;
    }

    if(i < NPROC){
      np->state = RUNNING;
      release(&proc_table_lock);
      break;
    }
    
    release(&proc_table_lock);
    np = &proc[0];
  }

  cpus[cpu()].lastproc = np;
  curproc[cpu()] = np;

  // h/w sets busy bit in TSS descriptor sometimes, and faults
  // if it's set in LTR. so clear tss descriptor busy bit.
  np->gdt[SEG_TSS].sd_type = STS_T32A;

  // XXX should probably have an lgdt() function in x86.h
  // to confine all the inline assembly.
  // XXX probably ought to lgdt on trap return too, in case
  // a system call has moved a program or changed its size.
  asm volatile("lgdt %0" : : "g" (np->gdt_pd.pd_lim));
  ltr(SEG_TSS << 3);

  if(0) cprintf("cpu%d: run %d esp=%p callerpc=%p\n", cpu(), np-proc);
  longjmp(&np->jmpbuf);
}

// give up the cpu by switching to the scheduler,
// which runs on the per-cpu stack.
void
swtch(int newstate)
{
  struct proc *p = curproc[cpu()];
  if(p == 0)
    panic("swtch");
  p->newstate = newstate; // basically an argument to scheduler()
  if(setjmp(&p->jmpbuf) == 0)
    longjmp(&cpus[cpu()].jmpbuf);
}

void
sleep(void *chan)
{
  struct proc *p = curproc[cpu()];
  if(p == 0)
    panic("sleep");
  p->chan = chan;
  swtch(WAITING);
}

void
wakeup(void *chan)
{
  struct proc *p;

  acquire(&proc_table_lock);
  for(p = proc; p < &proc[NPROC]; p++)
    if(p->state == WAITING && p->chan == chan)
      p->state = RUNNABLE;
  release(&proc_table_lock);
}

// give up the CPU but stay marked as RUNNABLE
void
yield()
{
  if(curproc[cpu()] == 0 || curproc[cpu()]->state != RUNNING)
    panic("yield");
  swtch(RUNNABLE);
}

void
proc_exit()
{
  struct proc *p;
  struct proc *cp = curproc[cpu()];
  int fd;

  cprintf("exit %x\n", cp);

  for(fd = 0; fd < NOFILE; fd++){
    if(cp->fds[fd]){
      fd_close(cp->fds[fd]);
      cp->fds[fd] = 0;
    }
  }

  acquire(&proc_table_lock);

  // wake up parent
  for(p = proc; p < &proc[NPROC]; p++)
    if(p->pid == cp->ppid)
      wakeup(p);

  // abandon children
  for(p = proc; p < &proc[NPROC]; p++)
    if(p->ppid == cp->pid)
      p->pid = 1;

  acquire(&proc_table_lock);

  // switch into scheduler
  swtch(ZOMBIE);
}

// disable interrupts
void
cli(void)
{
  cpus[cpu()].clis += 1;
  if(cpus[cpu()].clis == 1)
    __asm __volatile("cli");
}

// enable interrupts
void
sti(void)
{
  if(cpus[cpu()].clis < 1){
    cprintf("cpu %d clis %d\n", cpu(), cpus[cpu()].clis);
    panic("sti");
  }
  cpus[cpu()].clis -= 1;
  if(cpus[cpu()].clis < 1)
    __asm __volatile("sti");
}
