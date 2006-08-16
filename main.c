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
extern uchar _binary_init_start[], _binary_init_size[];

void main00();

// CPU 0 starts running C code here.
// This is called main0 not main so that it can have
// a void return type.  Gcc can't handle functions named
// main that don't return int.  Really.
void
main0(void)
{
  int i;
  struct proc *p;

  lcr4(0); // xxx copy of cpu #

  // clear BSS
  memset(edata, 0, end - edata);

  // switch to cpu0's cpu stack
  asm volatile("movl %0, %%esp" : : "r" (cpus[0].mpstack + MPSTACK - 32));
  asm volatile("movl %0, %%ebp" : : "r" (cpus[0].mpstack + MPSTACK));

  // Make sure interrupts stay disabled on all processors
  // until each signals it is ready, by pretending to hold
  // an extra lock.
  // xxx maybe replace w/ acquire remembering if FL_IF was already clear
  for(i=0; i<NCPU; i++){
    cpus[i].nlock++;
    cpus[i].guard1 = 0xdeadbeef;
    cpus[i].guard2 = 0xdeadbeef;
  }

  mp_init(); // collect info about this machine

  lapic_init(mp_bcpu());

  cprintf("\n\ncpu%d: booting xv6\n\n", cpu());

  pinit();
  binit();
  pic_init(); // initialize PIC
  ioapic_init();
  kinit(); // physical memory allocator
  tvinit(); // trap vectors
  idtinit(); // this CPU's idt register
  fd_init();
  iinit();

  // initialize process 0
  p = &proc[0];
  p->state = RUNNABLE;
  p->sz = 4 * PAGE;
  p->mem = kalloc(p->sz);
  memset(p->mem, 0, p->sz);
  p->kstack = kalloc(KSTACKSIZE);

  // cause proc[0] to start in kernel at main00
  memset(&p->jmpbuf, 0, sizeof p->jmpbuf);
  p->jmpbuf.eip = (uint)main00;
  p->jmpbuf.esp = (uint) (p->kstack + KSTACKSIZE - 4);

  // make sure there's a TSS
  setupsegs(0);

  // initialize I/O devices, let them enable interrupts
  console_init();
  ide_init(); 

  mp_startthem();

  // turn on timer and enable interrupts on the local APIC
  lapic_timerinit();
  lapic_enableintr();

  // Enable interrupts on this processor.
  cpus[cpu()].nlock--;
  sti();

  scheduler();
}

// Additional processors start here.
void
mpmain(void)
{
  lcr4(1); // xxx copy of cpu #

  cprintf("cpu%d: starting\n", cpu());
  idtinit(); // CPU's idt
  if(cpu() == 0)
    panic("mpmain on cpu 0");
  lapic_init(cpu());
  lapic_timerinit();
  lapic_enableintr();

  // make sure there's a TSS
  setupsegs(0);

  cpuid(0, 0, 0, 0, 0);	// memory barrier
  cpus[cpu()].booted = 1;

  // Enable interrupts on this processor.
  cpus[cpu()].nlock--;
  sti();

  scheduler();
}

// proc[0] starts here, called by scheduler() in the ordinary way.
void
main00()
{
  struct proc *p0 = &proc[0];
  struct proc *p1;
  extern struct spinlock proc_table_lock;
  struct trapframe tf;

  release(&proc_table_lock);

  p0->cwd = iget(rootdev, 1);
  iunlock(p0->cwd);

  // fake a trap frame as if a user process had made a system
  // call, so that copyproc will have a place for the new
  // process to return to.
  p0 = &proc[0];
  p0->tf = &tf;
  memset(p0->tf, 0, sizeof(struct trapframe));
  p0->tf->es = p0->tf->ds = p0->tf->ss = (SEG_UDATA << 3) | 3;
  p0->tf->cs = (SEG_UCODE << 3) | 3;
  p0->tf->eflags = FL_IF;
  p0->tf->esp = p0->sz;

  p1 = copyproc(&proc[0]);
  
  load_icode(p1, _binary_init_start, (uint) _binary_init_size);
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

  // Check magic number on binary
  elf = (struct elfhdr*) binary;
  if (elf->magic != ELF_MAGIC)
    panic("load_icode: not an ELF binary");

  p->tf->eip = elf->entry;

  // Map and load segments as directed.
  ph = (struct proghdr*) (binary + elf->phoff);
  for (i = 0; i < elf->phnum; i++, ph++) {
    if (ph->type != ELF_PROG_LOAD)
      continue;
    if (ph->va + ph->memsz < ph->va)
      panic("load_icode: overflow in elf header segment");
    if (ph->va + ph->memsz >= p->sz)
      panic("load_icode: icode wants to be above UTOP");

    // Load/clear the segment
    memmove(p->mem + ph->va, binary + ph->offset, ph->filesz);
    memset(p->mem + ph->va + ph->filesz, 0, ph->memsz - ph->filesz);
  }
}
