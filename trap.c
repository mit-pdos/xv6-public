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

  acquire_spinlock(&kernel_lock); // released in trapret in trapasm.S

  if(v == T_SYSCALL){
    curproc[cpu()]->tf = tf;
    syscall();
    return;
  }

  if(v == (IRQ_OFFSET + IRQ_TIMER)){
    curproc[cpu()]->tf = tf;
    lapic_timerintr();
    return;
  }

  cprintf("trap %d eip %x:%x\n", tf->tf_trapno, tf->tf_cs, tf->tf_eip);

  // XXX probably ought to lgdt on trap return

  return;
}
