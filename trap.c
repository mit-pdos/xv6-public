#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "traps.h"
#include "syscall.h"

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers

void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE << 3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 0, SEG_KCODE << 3, vectors[T_SYSCALL], DPL_USER);
}

void
idtinit(void)
{
  lidt(idt, sizeof idt);
}

void
trap(struct trapframe *tf)
{
  if(tf->trapno == T_SYSCALL){
    if(cp->killed)
      proc_exit();
    cp->tf = tf;
    syscall();
    if(cp->killed)
      proc_exit();
    return;
  }

  // PAGEBREAK: 10
  // Increment nlock to make sure interrupts stay off
  // during interrupt handler.  Decrement before returning.
  cpus[cpu()].nlock++;

  switch(tf->trapno){
  case IRQ_OFFSET + IRQ_TIMER:
    lapic_timerintr();
    cpus[cpu()].nlock--;
    if(cp){
      // Force process exit if it has been killed and is in user space.
      // (If it is still executing in the kernel, let it keep running
      // until it gets to the regular system call return.)
      if((tf->cs&3) == DPL_USER && cp->killed)
        proc_exit();

      // Force process to give up CPU and let others run.
      // If locks were held with interrupts on, would need to check nlock.
      if(cp->state == RUNNING)
        yield();
    }
    return;

  case IRQ_OFFSET + IRQ_IDE:
    ide_intr();
    lapic_eoi();
    break;
  
  case IRQ_OFFSET + IRQ_KBD:
    kbd_intr();
    lapic_eoi();
    break;
  
  case IRQ_OFFSET + IRQ_SPURIOUS:
    cprintf("spurious interrupt from cpu %d eip %x\n", cpu(), tf->eip);
    break;
    
  default:
    if(cp) {
      // Assume process divided by zero or dereferenced null, etc.
      cprintf("pid %d %s: unhandled trap %d on cpu %d eip %x -- kill proc\n",
              cp->pid, cp->name, tf->trapno, cpu(), tf->eip);
      proc_exit();
    }
    
    // Otherwise it's our mistake.
    cprintf("unexpected trap %d from cpu %d eip %x\n",
            tf->trapno, cpu(), tf->eip);
    panic("trap");
  }
  
  cpus[cpu()].nlock--;
}
