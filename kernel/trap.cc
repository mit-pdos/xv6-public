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
#include "kalloc.hh"
#include "apic.hh"
#include "kstream.hh"

extern "C" void __uaccess_end(void);

struct intdesc idt[256] __attribute__((aligned(16)));

// boot.S
extern u64 trapentry[];

u64
sysentry_c(u64 a0, u64 a1, u64 a2, u64 a3, u64 a4, u64 a5, u64 num)
{
  sti();

  if(myproc()->killed) {
    mtstart(trap, myproc());
    exit();
  }

  trapframe *tf = (trapframe*) (myproc()->kstack + KSTACKSIZE - sizeof(*tf));
  myproc()->tf = tf;
  u64 r = syscall(a0, a1, a2, a3, a4, a5, num);

  if(myproc()->killed) {
    mtstart(trap, myproc());
    exit();
  }

  return r;
}

int
do_pagefault(struct trapframe *tf)
{
  uptr addr = rcr2();
  if (myproc()->uaccess_) {
    if (addr >= USERTOP)
      panic("do_pagefault: %lx", addr);

    sti();
    if(pagefault(myproc()->vmap, addr, tf->err) >= 0){
#if MTRACE
      mtstop(myproc());
      if (myproc()->mtrace_stacks.curr >= 0)
        mtresume(myproc());
#endif
      return 0;
    }
    cprintf("pagefault: failed in kernel\n");
    tf->rax = -1;
    tf->rip = (u64)__uaccess_end;
    return 0;
  } else if (tf->err & FEC_U) {
      sti();
      if(pagefault(myproc()->vmap, addr, tf->err) >= 0){
#if MTRACE
        mtstop(myproc());
        if (myproc()->mtrace_stacks.curr >= 0)
          mtresume(myproc());
#endif
        return 0;
      }
      uerr.println("pagefault: failed in user");
      cli();
  }
  return -1;
}

void
trap(struct trapframe *tf)
{
  if (tf->trapno == T_NMI) {
    // The only locks that we can acquire during NMI are ones
    // we acquire only during NMI.
    if (sampintr(tf))
      return;
    panic("NMI");
  }

#if MTRACE
  if (myproc()->mtrace_stacks.curr >= 0)
    mtpause(myproc());
  mtstart(trap, myproc());
  // XXX mt_ascope ascope("trap:%d", tf->trapno);
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
      timerintr();
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
    lapiceoi();
    u64 nreq = tlbflush_req.load();
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

    if (tf->trapno == T_PGFLT && do_pagefault(tf) == 0)
      return;
      
    if (myproc() == 0 || (tf->cs&3) == 0)
      kerneltrap(tf);

    // In user space, assume process misbehaved.
    uerr.println("pid ", myproc()->pid, ' ', myproc()->name,
                 ": trap ", (u64)tf->trapno, " err ", (u32)tf->err,
                 " on cpu ", myid(), " rip ", shex(tf->rip),
                 " rsp ", shex(tf->rsp), " addr ", shex(rcr2()),
                 "--kill proc");
    myproc()->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running 
  // until it gets to the regular system call return.)
  if(myproc() && myproc()->killed && (tf->cs&3) == 0x3)
    exit();

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(myproc() && myproc()->get_state() == RUNNING && tf->trapno == T_IRQ0+IRQ_TIMER)
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
}

void
initnmi(void)
{
  void *nmistackbase = ksalloc(slab_stack);
  mycpu()->ts.ist[1] = (u64) nmistackbase + KSTACKSIZE;

  if (mycpu()->id == 0)
    idt[T_NMI].ist = 1;
}

void
initseg(void)
{
  volatile struct desctr dtr;
  struct cpu *c;

  dtr.limit = sizeof(idt) - 1;
  dtr.base = (u64)idt;
  lidt((void *)&dtr.limit);

  // TLS might not be ready
  c = &cpus[myid()];
  // Load per-CPU GDT
  memmove(c->gdt, bootgdt, sizeof(bootgdt));
  dtr.limit = sizeof(c->gdt) - 1;
  dtr.base = (u64)c->gdt;
  lgdt((void *)&dtr.limit);

  // When executing a syscall instruction the CPU sets the SS selector
  // to (star >> 32) + 8 and the CS selector to (star >> 32).
  // When executing a sysret instruction the CPU sets the SS selector
  // to (star >> 48) + 8 and the CS selector to (star >> 48) + 16.
  u64 star = ((((u64)UCSEG|0x3) - 16)<<48)|((u64)KCSEG<<32);
  writemsr(MSR_STAR, star);
  writemsr(MSR_LSTAR, (u64)&sysentry);
  writemsr(MSR_SFMASK, FL_TF | FL_IF);
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
    if(rbp == 0 || rbp < (uptr*)KBASE || rbp == (uptr*)(~0UL) ||
       (rbp >= (uptr*)KBASEEND && rbp < (uptr*)KCODE))
      break;
    pcs[i] = rbp[1];     // saved %rip
    rbp = (uptr*)rbp[0]; // saved %rbp
  }
  for(; i < n; i++)
    pcs[i] = 0;
}
