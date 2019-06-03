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

void kerneltrap();

void
trapinit(void)
{
  int i;

  // send interrupts and exceptions to kerneltrap().
  w_stvec((uint64)kerneltrap);

  // set up the riscv Platform Level Interrupt Controller
  // to send uart interrupts to hart 0 S-Mode.

  // qemu makes UART0 be interrupt number 10.
  int irq = 10;
  // set uart's priority to be non-zero (otherwise disabled).
  *(uint*)(0x0c000000L + irq*4) = 1;
  // set uart's enable bit for hart 0 S-mode. 
  *(uint*)0x0c002080 = (1 << irq);

  // set hart 0 S-mode priority threshold to 0.
  *(uint*)0x0c201000 = 0;

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
  w_stvec((uint64)kerneltrap);

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
  } else {
    printf("usertrap(): unexpected scause 0x%x pid=%d\n", r_scause(), p->pid);
    printf("            sepc=%p stval=%p\n", r_sepc(), r_stval());
    panic("usertrap");
  }

  usertrapret();
}

//
// return to user space
//
void
usertrapret(void)
{
  struct proc *p = myproc();

  // XXX turn off interrupts, since we're switching
  // now from kerneltrap() to usertrap().

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
void __attribute__ ((aligned (4)))
kerneltrap()
{
  if((r_sstatus() & SSTATUS_SPP) == 0)
    panic("kerneltrap: not from supervisor mode");

  printf("scause 0x%x\n", r_scause());
  printf("sepc=%p stval=%p\n", r_sepc(), r_stval());

  panic("kerneltrap");
}
