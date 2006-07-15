#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "traps.h"
#include "syscall.h"

struct Gatedesc idt[256];
struct Pseudodesc idt_pd = { 0, sizeof(idt) - 1, (unsigned) &idt };
extern unsigned vectors[]; /* vectors.S, array of 256 entry point addresses */

extern void trapenter();
extern void trapenter1();

void
tvinit()
{
  int i;

  for(i = 0; i < 256; i++){
    SETGATE(idt[i], 1, SEG_KCODE << 3, vectors[i], 0);
  }
  SETGATE(idt[T_SYSCALL], T_SYSCALL, SEG_KCODE << 3, vectors[48], 3);
}

void
idtinit()
{
  asm volatile("lidt %0" : : "g" (idt_pd.pd_lim));
}

void
trap(struct Trapframe *tf)
{
  int v = tf->tf_trapno;

  if(cpus[cpu()].clis){
    cprintf("cpu %d v %d eip %x\n", cpu(), v, tf->tf_eip);
    panic("interrupt while interrupts are off");
  }

  if(v == T_SYSCALL){
    struct proc *cp = curproc[cpu()];
    int num = cp->tf->tf_regs.reg_eax;
    if(cp == 0)
      panic("syscall with no proc");
    if(cp->killed)
      proc_exit();
    cp->tf = tf;
    syscall();
    if(cp != curproc[cpu()])
      panic("trap ret wrong curproc");
    if(cp->state != RUNNING)
      panic("trap ret but not RUNNING");
    if(tf != cp->tf)
      panic("trap ret wrong tf");
    if(cp->locks){
      cprintf("num=%d\n", num);
      panic("syscall returning locks held");
    }
    if(cpus[cpu()].clis)
      panic("syscall returning but clis != 0");
    if((read_eflags() & FL_IF) == 0)
      panic("syscall returning but FL_IF clear");
    if(read_esp() < (unsigned)cp->kstack ||
       read_esp() >= (unsigned)cp->kstack + KSTACKSIZE)
      panic("trap ret esp wrong");
    if(cp->killed)
      proc_exit();
    return;
  }

  if(v == (IRQ_OFFSET + IRQ_TIMER)){
    struct proc *cp = curproc[cpu()];
    lapic_timerintr();
    if(cp && cp->locks)
      panic("timer interrupt while holding a lock");
    if(cp){
#if 1
      if((read_eflags() & FL_IF) == 0)
        panic("timer interrupt but interrupts now disabled");
#else
      cpus[cpu()].clis += 1;
      sti();
#endif
      if(cp->killed)
        proc_exit();
      if(cp->state == RUNNING)
        yield();
    }
    return;
  }

  if(v == (IRQ_OFFSET + IRQ_IDE)){
    ide_intr();
    return;
  }


  // XXX probably ought to lgdt on trap return

  return;
}
