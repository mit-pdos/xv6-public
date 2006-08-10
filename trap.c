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
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE << 3, vectors[48], 3);
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
  
  if(cpus[cpu()].nlock){
    cprintf("trap v %d eip %x cpu %d nlock %d\n",
            v, tf->eip, cpu(), cpus[cpu()].nlock);
    panic("interrupt while holding a lock");
  }

  if(cpu() == 1 && curproc[cpu()] == 0){
    if(&tf < cpus[cpu()].mpstack || &tf > cpus[cpu()].mpstack + 512){
      cprintf("&tf %x mpstack %x\n", &tf, cpus[cpu()].mpstack);
      panic("trap cpu stack");
    }
  } else if(curproc[cpu()]){
    if(&tf < curproc[cpu()]->kstack){
      panic("trap kstack");
    }
  }

  if(v == T_SYSCALL){
    struct proc *cp = curproc[cpu()];
    int num = cp->tf->eax;
    if((read_eflags() & FL_IF) == 0)
      panic("syscall but interrupts now disabled");
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
    if(cpus[cpu()].nlock){
      cprintf("num=%d\n", num);
      panic("syscall returning locks held");
    }
    if((read_eflags() & FL_IF) == 0)
      panic("syscall returning but FL_IF clear");
    if(read_esp() < (uint)cp->kstack ||
       read_esp() >= (uint)cp->kstack + KSTACKSIZE)
      panic("trap ret esp wrong");
    if(cp->killed)
      proc_exit();
    // XXX probably ought to lgdt on trap return
    return;
  }

  //if(read_eflags() & FL_IF)
  //panic("interrupt but interrupts enabled");

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
    if(cpus[cpu()].nlock)
      panic("ide_intr returned while holding a lock");
    cli(); // prevent a waiting interrupt from overflowing stack
    lapic_eoi();
    return;
  }

  if(v == (IRQ_OFFSET + IRQ_KBD)){
    kbd_intr();
    if(cpus[cpu()].nlock){
      panic("kbd_intr returned while holding a lock");
    }
    cli(); // prevent a waiting interrupt from overflowing stack
    lapic_eoi();
    return;
  }

  cprintf("trap %d\n", v);

  return;
}
