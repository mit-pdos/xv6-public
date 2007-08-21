#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "traps.h"
#include "syscall.h"
#include "elf.h"
#include "param.h"
#include "spinlock.h"

extern char edata[], end[];

void proc0init();

// Bootstrap processor starts running C code here.
// This is called main0 not main so that it can have
// a void return type.  Gcc can't handle functions named
// main that don't return int.  Really.
void
main0(void)
{
  int i;
  static int bcpu;  // cannot be on stack

  // clear BSS
  memset(edata, 0, end - edata);

  // Prevent release() from enabling interrupts.
  for(i=0; i<NCPU; i++)
    cpus[i].nlock = 1;

  mp_init(); // collect info about this machine
  bcpu = mp_bcpu();

  // switch to bootstrap processor's stack
  asm volatile("movl %0, %%esp" : : "r" (cpus[bcpu].mpstack + MPSTACK - 32));
  asm volatile("movl %0, %%ebp" : : "r" (cpus[bcpu].mpstack + MPSTACK));

  lapic_init(bcpu);

  cprintf("\ncpu%d: starting xv6\n\n", cpu());

  pinit(); // process table
  binit(); // buffer cache
  pic_init();
  ioapic_init();
  kinit(); // physical memory allocator
  tvinit(); // trap vectors
  idtinit(); // this CPU's interrupt descriptor table
  fileinit();
  iinit(); // i-node table

  // make sure there's a TSS
  setupsegs(0);

  // initialize I/O devices, let them enable interrupts
  console_init();
  ide_init();

  // start other CPUs
  mp_startthem();

  // turn on timer
  if(ismp)
    lapic_timerinit();
  else
    pit8253_timerinit();

  // enable interrupts on the local APIC
  lapic_enableintr();

  // enable interrupts on this processor.
  cpus[cpu()].nlock--;
  sti();

  // initialize process 0
  proc0init();

  scheduler();
}

// Additional processors start here.
void
mpmain(void)
{
  cprintf("cpu%d: starting\n", cpu());
  idtinit(); // CPU's idt
  if(cpu() == 0)
    panic("mpmain on cpu 0");
  lapic_init(cpu());
  lapic_timerinit();
  lapic_enableintr();

  // make sure there's a TSS
  setupsegs(0);

  cpuid(0, 0, 0, 0, 0);  // memory barrier
  cpus[cpu()].booted = 1;

  // Enable interrupts on this processor.
  cpus[cpu()].nlock--;
  sti();

  scheduler();
}

void
proc0init(void)
{
  struct proc *p;
  extern uchar _binary_initcode_start[], _binary_initcode_size[];
  
  p = copyproc(0);
  p->sz = PAGE;
  p->mem = kalloc(p->sz);
  p->cwd = igetroot();
  memset(&p->tf, 0, sizeof p->tf);
  p->tf->es = p->tf->ds = p->tf->ss = (SEG_UDATA << 3) | DPL_USER;
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
  p->tf->eflags = FL_IF;
  p->tf->esp = p->sz;
  
  // Push dummy return address to placate gcc.
  p->tf->esp -= 4;
  *(uint*)(p->mem + p->tf->esp) = 0xefefefef;

  p->tf->eip = 0;
  memmove(p->mem, _binary_initcode_start, (int)_binary_initcode_size);
  safestrcpy(p->name, "initcode", sizeof p->name);
  p->state = RUNNABLE;
}

