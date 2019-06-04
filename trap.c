#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "proc.h"
#include "spinlock.h"
#include "defs.h"

struct spinlock tickslock;
uint ticks;

extern char trampout[], trampin[];

// in kernelvec.S, calls kerneltrap().
void kernelvec();

extern int devintr();

void
trapinit(void)
{
  int i;

  // set up to take exceptions and traps while in the kernel.
  w_stvec((uint64)kernelvec);

  // time, cycle, instret CSRs

  initlock(&tickslock, "time");
}

//
// handle an interrupt, exception, or system call from user space.
// called from trampoline.S
//
void
usertrap(void)
{
  if((r_sstatus() & SSTATUS_SPP) != 0)
    panic("usertrap: not from user mode");

  // send interrupts and exceptions to kerneltrap(),
  // since we're now in the kernel.
  w_stvec((uint64)kernelvec);

  //printf("mtimecmp %x mtime %x\n", *(uint64*)CLINT_MTIMECMP0, *(uint64*)CLINT_MTIME);

  *(uint64*)CLINT_MTIMECMP0 = *(uint64*)CLINT_MTIME + 10000;

  struct proc *p = myproc();
  
  // save user program counter.
  p->tf->epc = r_sepc();

  intr_on();
  
  if(r_scause() == 8){
    // system call

    // sepc points to the ecall instruction,
    // but we want to return to the next instruction.
    p->tf->epc += 4;

    syscall();
  } else if(devintr()){
    // ok
  } else {
    printf("usertrap(): unexpected scause 0x%p pid=%d\n", r_scause(), p->pid);
    printf("            sepc=%p stval=%p\n", r_sepc(), r_stval());
    p->killed = 1;
  }

  if(p->killed)
    exit();

  usertrapret();
}

//
// return to user space
//
void
usertrapret(void)
{
  struct proc *p = myproc();

  // turn off interrupts, since we're switching
  // now from kerneltrap() to usertrap().
  intr_off();

  // send interrupts and exceptions to trampoline.S
  w_stvec(TRAMPOLINE + (trampin - trampout));

  // set up values that trampoline.S will need when
  // the process next re-enters the kernel.
  p->tf->kernel_satp = r_satp();
  p->tf->kernel_sp = (uint64)p->kstack + PGSIZE;
  p->tf->kernel_trap = (uint64)usertrap;

  // set up the registers that trampoline.S's sret will use
  // to get to user space.
  
  // set S Previous Privilege mode to User.
  unsigned long x = r_sstatus();
  x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
  x |= SSTATUS_SPIE; // enable interrupts in user mode
  w_sstatus(x);

  // set S Exception Program Counter to the saved user pc.
  w_sepc(p->tf->epc);

  // tell trampline.S the user page table to switch to.
  uint64 satp = MAKE_SATP(p->pagetable);

  // jump to trampoline.S at the top of memory, which 
  // switches to the user page table, restores user registers,
  // and switches to user mode with sret.
  ((void (*)(uint64,uint64))TRAMPOLINE)(TRAMPOLINE - PGSIZE, satp);
}

// interrupts and exceptions from kernel code go here,
// on whatever the current kernel stack is.
// must be 4-byte aligned to fit in stvec.
void 
kerneltrap()
{
  uint64 sstatus = r_sstatus();
  uint64 scause = r_scause();
  
  if((sstatus & SSTATUS_SPP) == 0)
    panic("kerneltrap: not from supervisor mode");

  if(devintr() == 0){
    printf("scause 0x%p\n", scause);
    printf("sepc=%p stval=%p\n", r_sepc(), r_stval());
    panic("kerneltrap");
  }

  // turn off interrupts to ensure we
  // return with the correct sstatus.
  intr_off();

  // restore previous interrupt status.
  w_sstatus(sstatus);
}

// check if it's an external interrupt or software interrupt,
// and handle it.
// returns 1 if handled, 0 if not recognized.
int
devintr()
{
  uint64 scause = r_scause();

  if((scause & 0x8000000000000000L) &&
     (scause & 0xff) == 9){
    // supervisor external interrupt, via PLIC.
    int irq = plic_claim();

    if(irq == UART0_IRQ){
      uartintr();
    } else {
      printf("stray interrupt irq=%d\n", irq);
    }

    plic_complete(irq);
    return 1;
  } else if(scause == 0x8000000000000001){
    // software interrupt from a machine-mode timer interrupt.

    acquire(&tickslock);
    ticks++;
    wakeup(&ticks);
    release(&tickslock);
    
    // acknowledge.
    w_sip(r_sip() & ~2);

    return 1;
  } else {
    return 0;
  }
}

