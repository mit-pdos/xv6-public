#include "types.h"
#include "param.h"
#include "mmu.h"
#include "kernel.h"
#include "amd64.h"
#include "cpu.h"
#include "traps.h"
#include "queue.h"
#include "spinlock.h"
#include "condvar.h"
#include "proc.h"
#include "kmtrace.h"
#include "bits.h"

u64 ticks __mpalign__;

struct spinlock tickslock __mpalign__;
struct condvar cv_ticks __mpalign__;

struct segdesc  __attribute__((aligned(16))) bootgdt[NSEGS] = {
  // null
  [0]=SEGDESC(0, 0, 0),
  // 32-bit kernel code
  [1]=SEGDESC(0, 0xfffff, SEG_R|SEG_CODE|SEG_S|SEG_DPL(0)|SEG_P|SEG_D|SEG_G),
  // 64-bit kernel code
  [2]=SEGDESC(0, 0, SEG_R|SEG_CODE|SEG_S|SEG_DPL(0)|SEG_P|SEG_L|SEG_G),
  // kernel data
  [3]=SEGDESC(0, 0xfffff, SEG_W|SEG_S|SEG_DPL(0)|SEG_P|SEG_D|SEG_G),
  // The order of the user data and user code segments is
  // important for syscall instructions.  See initseg.
  // 64-bit user data
  [6]=SEGDESC(0, 0xfffff, SEG_W|SEG_S|SEG_DPL(3)|SEG_P|SEG_D|SEG_G),
  // 64-bit user code
  [7]=SEGDESC(0, 0, SEG_R|SEG_CODE|SEG_S|SEG_DPL(3)|SEG_P|SEG_L|SEG_G),  
};

struct intdesc idt[256] __attribute__((aligned(16)));

// boot.S
extern u64 trapentry[];

void
trap(struct trapframe *tf)
{
  // XXX(sbw) eventually these should be moved into trapasm.S
  cli();
  writegs(KDSEG);
  writemsr(MSR_GS_BASE, (u64)&cpus[cpunum()].cpu);
  sti();

  // XXX(sbw) sysenter/sysexit
  if(tf->trapno == T_SYSCALL){
    if(myproc()->killed) {
      mtstart(trap, myproc());
      exit();
    }
    myproc()->tf = tf;
    syscall();
    if(myproc()->killed) {
      mtstart(trap, myproc());
      exit();
    }
    return;
  }

#if MTRACE
  if (myproc()->mtrace_stacks.curr >= 0)
    mtpause(myproc());
  mtstart(trap, myproc());
#endif

  switch(tf->trapno){
  case T_IRQ0 + IRQ_TIMER:
    if(mycpu()->id == 0){
      acquire(&tickslock);
      ticks++;
      cv_wakeup(&cv_ticks);
      release(&tickslock);
    }
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE:
    ideintr();
    lapiceoi();
    piceoi();
    break;
  case T_IRQ0 + IRQ_IDE+1:
    // Bochs generates spurious IDE1 interrupts.
    break;
  case T_IRQ0 + IRQ_KBD:
    kbdintr();
    lapiceoi();
    piceoi();
    break;
  case T_IRQ0 + IRQ_COM1:
    uartintr();
    lapiceoi();
    piceoi();
    break;
  case T_IRQ0 + 7:
  case T_IRQ0 + IRQ_SPURIOUS:
    cprintf("cpu%d: spurious interrupt at %x:%lx\n",
            mycpu()->id, tf->cs, tf->rip);
    lapiceoi();
  case T_TLBFLUSH:
    lapiceoi();
    lcr3(rcr3());
    break;
   
  //PAGEBREAK: 13
  default:
    if(myproc() == 0 || (tf->cs&3) == 0){
      // In kernel, it must be our mistake.
      cprintf("unexpected trap %d from cpu %d rip %lx (cr2=0x%lx)\n",
              tf->trapno, mycpu()->id, tf->rip, rcr2());
      panic("trap");
    }

    if(tf->trapno == T_PGFLT){
      uptr addr = rcr2();
      if(addr < USERTOP && pagefault(myproc()->vmap, addr, tf->err) >= 0){
#if MTRACE
        mtstop(myproc());
        if (myproc()->mtrace_stacks.curr >= 0)
          mtresume(myproc());
#endif
        return;
      }
    }

    // In user space, assume process misbehaved.
    cprintf("pid %d %s: trap %d err %d on cpu %d "
            "eip 0x%x addr 0x%x--kill proc\n",
            myproc()->pid, myproc()->name, tf->trapno, tf->err,
            mycpu()->id, tf->rip, rcr2());
    myproc()->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running 
  // until it gets to the regular system call return.)
  if(myproc() && myproc()->killed && (tf->cs&3) == 0x3)
    exit();

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(myproc() && myproc()->state == RUNNING && tf->trapno == T_IRQ0+IRQ_TIMER)
    yield();

  // Check if the process has been killed since we yielded
  if(myproc() && myproc()->killed && (tf->cs&3) == 0x3)
    exit();

#if MTRACE
  mtstop(myproc());
  if (myproc()->mtrace_stacks.curr >= 0)
    mtresume(myproc());
#endif
}

void
inittrap(void)
{
  uint64 entry;
  uint32 bits;
  int i;
  
  bits = INT_P | SEG_INTR64;  // present, interrupt gate
  for(i=0; i<256; i++) {
    entry = trapentry[i];
    idt[i] = INTDESC(KCSEG, entry, bits);
  }
  entry = trapentry[T_SYSCALL];
  idt[T_SYSCALL] = INTDESC(KCSEG, entry, SEG_DPL(3) | SEG_TRAP64 |INT_P);
}

void
initseg(void)
{
  extern void sysentry(void);
  volatile struct desctr dtr;
  struct cpu *c;

  dtr.limit = sizeof(idt) - 1;
  dtr.base = (u64)idt;
  lidt((void *)&dtr.limit);

  // TLS might not be ready
  c = &cpus[cpunum()];
  // Load per-CPU GDT
  memmove(c->gdt, bootgdt, sizeof(bootgdt));
  dtr.limit = sizeof(c->gdt) - 1;
  dtr.base = (u64)c->gdt;
  lgdt((void *)&dtr.limit);

#if 0
  // When executing a syscall instruction the CPU sets the SS selector
  // to (star >> 32) + 8 and the CS selector to (star >> 32).
  // When executing a sysret instruction the CPU sets the SS selector
  // to (star >> 48) + 8 and the CS selector to (star >> 48) + 16.
  u64 star = ((((u64)UCSEG|0x3) - 16)<<48)|((u64)KCSEG<<32);
  writemsr(MSR_STAR, star);
  writemsr(MSR_LSTAR, (u64)&sysentry);
  writemsr(MSR_SFMASK, FL_TF);
#endif
}
