#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "traps.h"
#include "syscall.h"

struct gatedesc idt[256];
extern uint vectors[]; /* vectors.S, array of 256 entry point addresses */

extern void trapenter(void);
extern void trapenter1(void);

void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++){
    SETGATE(idt[i], 1, SEG_KCODE << 3, vectors[i], 0);
  }
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE << 3, vectors[T_SYSCALL], 3);
}

void
idtinit(void)
{
  lidt(idt, sizeof idt);
}

void
trap(struct trapframe *tf)
{
  int v = tf->trapno;

  if(v == T_SYSCALL){
    struct proc *cp = curproc[cpu()];
    if(cp->killed)
      proc_exit();
    cp->tf = tf;
    syscall();
    if(cp->killed)
      proc_exit();
    return;
  }

  if(v == (IRQ_OFFSET + IRQ_TIMER)){
    struct proc *cp = curproc[cpu()];
    lapic_timerintr();
    if(cp){
      // Force process exit if it has been killed
      // and the interrupt came from user space.
      // (If the kernel was executing at time of interrupt,
      // don't kill the process.  Let the process get back
      // out to its regular system call return.)
      if((tf->cs&3) == 3 && cp->killed)
        proc_exit();
      
      // Force process to give up CPU and let others run.
      if(cp->state == RUNNING)
        yield();
    }
    return;
  }

  if(v == (IRQ_OFFSET + IRQ_IDE)){
    ide_intr();
    cli(); // prevent a waiting interrupt from overflowing stack
    lapic_eoi();
    return;
  }

  if(v == (IRQ_OFFSET + IRQ_KBD)){
    kbd_intr();
    cli(); // prevent a waiting interrupt from overflowing stack
    lapic_eoi();
    return;
  }

  if(v == (IRQ_OFFSET + IRQ_SPURIOUS)){
    cprintf("spurious interrupt from cpu %d eip %x\n", cpu(), tf->eip);
    return;  // no eoi for this one.
  }

  if(curproc[cpu()]) {
    cprintf("pid %d: unhandled trap %d on cpu %d eip %x---terminate process\n", 
            curproc[cpu()]->pid, v, cpu(), tf->eip);
    proc_exit();
  }
  cprintf("unexpected trap %d from cpu %d eip %x\n", v, cpu(), tf->eip);
  panic("trap");
}
