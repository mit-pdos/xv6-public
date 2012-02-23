#include "types.h"
#include "mmu.h"
#include "kernel.hh"
#include "amd64.h"
#include "cpu.hh"
#include "traps.h"
#include "queue.h"
#include "spinlock.h"
#include "condvar.h"
#include "proc.hh"
#include "kmtrace.hh"
#include "bits.hh"

struct intdesc idt[256] __attribute__((aligned(16)));

// boot.S
extern u64 trapentry[];

void
trap(struct trapframe *tf)
{
  writegs(KDSEG);
  writemsr(MSR_GS_BASE, (u64)&cpus[cpunum()].cpu);

  if (tf->trapno == T_NMI) {
    // The only locks that we can acquire during NMI are ones
    // we acquire only during NMI.
    if (sampintr(tf))
      return;
    panic("NMI");
  }

  // XXX(sbw) sysenter/sysexit
  if(tf->trapno == T_SYSCALL){
    sti();
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
    if (mycpu()->timer_printpc) {
      cprintf("cpu%d: proc %s rip %lx rsp %lx cs %x\n",
              mycpu()->id,
              myproc() ? myproc()->name : "(none)",
              tf->rip, tf->rsp, tf->cs);
      if (mycpu()->timer_printpc == 2 && tf->rbp > KBASE) {
        uptr pc[10];
        getcallerpcs((void *) tf->rbp, pc, NELEM(pc));
        for (int i = 0; i < 10 && pc[i]; i++)
          cprintf("cpu%d:   %lx\n", mycpu()->id, pc[i]);
      }
      mycpu()->timer_printpc = 0;
    }
    if (mycpu()->id == 0)
      cv_tick();
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
  case T_IRQ0 + IRQ_COM2:
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
    break;
  case T_IRQ0 + IRQ_ERROR:
    cprintf("cpu%d: lapic error?\n", mycpu()->id);
    lapiceoi();
    break;
  case T_TLBFLUSH: {
    u64 nreq = tlbflush_req.load();
    lapiceoi();
    lcr3(rcr3());
    mycpu()->tlbflush_done = nreq;
    break;
  }
  case T_SAMPCONF:
    lapiceoi();
    sampconf();  
    break;
  default:
    if (tf->trapno == T_IRQ0+e1000irq) {
      e1000intr();
      lapiceoi();
      piceoi();
      break;
    }

    if (myproc() == 0 || (tf->cs&3) == 0)
      kerneltrap(tf);

    if(tf->trapno == T_PGFLT){
      uptr addr = rcr2();
      sti();
      if(pagefault(myproc()->vmap, addr, tf->err) >= 0){
#if MTRACE
        mtstop(myproc());
        if (myproc()->mtrace_stacks.curr >= 0)
          mtresume(myproc());
#endif
        return;
      }
      cli();
    }

    // In user space, assume process misbehaved.
    cprintf("pid %d %s: trap %lu err %d on cpu %d "
            "rip 0x%lx rsp 0x%lx addr 0x%lx--kill proc\n",
            myproc()->pid, myproc()->name, tf->trapno, tf->err,
            mycpu()->id, tf->rip, tf->rsp, rcr2());
    myproc()->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running 
  // until it gets to the regular system call return.)
  if(myproc() && myproc()->killed && (tf->cs&3) == 0x3)
    exit();

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(myproc() && get_proc_state(myproc()) == RUNNING && tf->trapno == T_IRQ0+IRQ_TIMER)
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
  u64 entry;
  u8 bits;
  int i;
  
  bits = INT_P | SEG_INTR64;  // present, interrupt gate
  for(i=0; i<256; i++) {
    entry = trapentry[i];
    idt[i] = INTDESC(KCSEG, entry, bits);
  }
  entry = trapentry[T_SYSCALL];
  idt[T_SYSCALL] = INTDESC(KCSEG, entry, SEG_DPL(3) | SEG_INTR64 |INT_P);
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

// Pushcli/popcli are like cli/sti except that they are matched:
// it takes two popcli to undo two pushcli.  Also, if interrupts
// are off, then pushcli, popcli leaves them off.
void
pushcli(void)
{
  u64 rflags;

  rflags = readrflags();
  cli();
  if(mycpu()->ncli++ == 0)
    mycpu()->intena = rflags & FL_IF;
}

void
popcli(void)
{
  if(readrflags()&FL_IF)
    panic("popcli - interruptible");
  if(--mycpu()->ncli < 0)
    panic("popcli");
  if(mycpu()->ncli == 0 && mycpu()->intena)
    sti();
}

// Record the current call stack in pcs[] by following the %rbp chain.
void
getcallerpcs(void *v, uptr pcs[], int n)
{
  uptr *rbp;
  int i;

  rbp = (uptr*)v;
  for(i = 0; i < n; i++){
    if(rbp == 0 || rbp < (uptr*)KBASE || rbp == (uptr*)(~0UL))
      break;
    pcs[i] = rbp[1];     // saved %rip
    rbp = (uptr*)rbp[0]; // saved %rbp
  }
  for(; i < n; i++)
    pcs[i] = 0;
}
