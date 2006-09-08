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
extern uchar _binary__init_start[], _binary__init_size[];

void process0();

// Bootstrap processor starts running C code here.
// This is called main0 not main so that it can have
// a void return type.  Gcc can't handle functions named
// main that don't return int.  Really.
void
main0(void)
{
  int i;
  int bcpu;
  struct proc *p;

  // clear BSS
  memset(edata, 0, end - edata);

  // Prevent release() from enabling interrupts.
  for(i=0; i<NCPU; i++)
    cpus[i].nlock = 1;

  mp_init(); // collect info about this machine
  bcpu = mp_bcpu();

  // switch to bootstrap processor's stack
  asm volatile("movl %0, %%esp" : : "r" (cpus[0].mpstack + MPSTACK - 32));
  asm volatile("movl %0, %%ebp" : : "r" (cpus[0].mpstack + MPSTACK));

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

  // initialize process 0
  p = &proc[0];
  p->state = RUNNABLE;
  p->kstack = kalloc(KSTACKSIZE);

  // cause proc[0] to start in kernel at process0
  p->jmpbuf.eip = (uint) process0;
  p->jmpbuf.esp = (uint) (p->kstack + KSTACKSIZE - 4);

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

// proc[0] starts here, called by scheduler() in the ordinary way.
void
process0()
{
  struct proc *p0 = &proc[0];
  struct proc *p1;
  extern struct spinlock proc_table_lock;
  struct trapframe tf;

  release(&proc_table_lock);

  p0->cwd = iget(rootdev, 1);
  iunlock(p0->cwd);

  // dummy user memory to make copyproc() happy.
  // must be big enough to hold the init binary.
  p0->sz = PAGE;
  p0->mem = kalloc(p0->sz);

  // fake a trap frame as if a user process had made a system
  // call, so that copyproc will have a place for the new
  // process to return to.
  p0->tf = &tf;
  memset(p0->tf, 0, sizeof(struct trapframe));
  p0->tf->es = p0->tf->ds = p0->tf->ss = (SEG_UDATA << 3) | 3;
  p0->tf->cs = (SEG_UCODE << 3) | 3;
  p0->tf->eflags = FL_IF;
  p0->tf->esp = p0->sz;

  p1 = copyproc(p0);

  load_icode(p1, _binary__init_start, (uint) _binary__init_size);
  p1->state = RUNNABLE;

  proc_wait();
  panic("init exited");
}

void
load_icode(struct proc *p, uchar *binary, uint size)
{
  int i;
  struct elfhdr *elf;
  struct proghdr *ph;

  elf = (struct elfhdr*) binary;
  if(elf->magic != ELF_MAGIC)
    panic("load_icode: not an ELF binary");

  p->tf->eip = elf->entry;

  // Map and load segments as directed.
  ph = (struct proghdr*) (binary + elf->phoff);
  for(i = 0; i < elf->phnum; i++, ph++) {
    if(ph->type != ELF_PROG_LOAD)
      continue;
    if(ph->va + ph->memsz < ph->va)
      panic("load_icode: overflow in proghdr");
    if(ph->va + ph->memsz >= p->sz)
      panic("load_icode: icode too large");

    // Load/clear the segment
    memmove(p->mem + ph->va, binary + ph->offset, ph->filesz);
    memset(p->mem + ph->va + ph->filesz, 0, ph->memsz - ph->filesz);
  }
}
