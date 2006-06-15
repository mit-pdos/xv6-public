#include "types.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "param.h"
#include "defs.h"

struct proc proc[NPROC];
struct proc *curproc;

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
 */
struct proc *
newproc()
{
  struct proc *np;
  unsigned *sp;

  for(np = &proc[1]; np < &proc[NPROC]; np++)
    if(np->state == UNUSED)
      break;
  if(np >= &proc[NPROC])
    return 0;

  np->sz = curproc->sz;
  np->mem = kalloc(curproc->sz);
  if(np->mem == 0)
    return 0;
  memcpy(np->mem, curproc->mem, np->sz);
  np->kstack = kalloc(KSTACKSIZE);
  if(np->kstack == 0){
    kfree(np->mem, curproc->sz);
    return 0;
  }
  setupsegs(np);
  
  // set up kernel stack to return to user space
  np->tf = (struct Trapframe *) (np->kstack + KSTACKSIZE - sizeof(struct Trapframe));
  *(np->tf) = *(curproc->tf);
  sp = (unsigned *) np->tf;
  *(--sp) = (unsigned) &trapret;  // for return from swtch()
  *(--sp) = 0;  // previous bp for leave in swtch()
  np->esp = (unsigned) sp;
  np->ebp = (unsigned) sp;

  np->state = RUNNABLE;

  cprintf("newproc %x\n", np);

  return np;
}

/*
 * find a runnable process and switch to it.
 */
void
swtch()
{
  struct proc *np;
  
  while(1){
    for(np = curproc + 1; np != curproc; np++){
      if(np == &proc[NPROC])
        np = &proc[0];
      if(np->state == RUNNABLE)
        break;
    }
    if(np->state == RUNNABLE)
      break;
    // idle...
  }
  
  curproc->ebp = read_ebp();
  curproc->esp = read_esp();

  cprintf("swtch %x -> %x\n", curproc, np);

  curproc = np;

  // XXX callee-saved registers?

  // XXX probably ought to lgdt on trap return too

  asm volatile("lgdt %0" : : "g" (np->gdt_pd.pd_lim));
  ltr(SEG_TSS << 3);

  // this happens to work, but probably isn't safe:
  // it's not clear that np->ebp is guaranteed to evaluate
  // correctly after changing the stack pointer.
  asm volatile("movl %0, %%esp" : : "g" (np->esp));
  asm volatile("movl %0, %%ebp" : : "g" (np->ebp));
}
