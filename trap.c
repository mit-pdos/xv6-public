#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "traps.h"

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

  if(tf->tf_cs == 0x8 && kernel_lock == cpu())
    cprintf("cpu %d: trap from %x:%x with lock=%d\n",
            cpu(), tf->tf_cs, tf->tf_eip, kernel_lock);

  acquire_spinlock(&kernel_lock); // released in trapret in trapasm.S

  if(v == T_SYSCALL){
    struct proc *cp = curproc[cpu()];
    cp->tf = tf;
    syscall();
    if(cp != curproc[cpu()])
      panic("trap ret wrong curproc");
    if(cp->state != RUNNING)
      panic("trap ret but not RUNNING");
    if(tf != cp->tf)
      panic("trap ret wrong tf");
    if(read_esp() < cp->kstack || read_esp() >= cp->kstack + KSTACKSIZE)
      panic("trap ret esp wrong");
    return;
  }

  if(v == (IRQ_OFFSET + IRQ_TIMER)){
    lapic_timerintr();
    return;
  }


  // XXX probably ought to lgdt on trap return

  return;
}
