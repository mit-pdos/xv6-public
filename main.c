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
extern uchar _binary_userfs_start[], _binary_userfs_size[];
extern uchar _binary_init_start[], _binary_init_size[];

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

  // Make sure interrupts stay disabled on all processors
  // until each signals it is ready, by pretending to hold
  // an extra lock.
  // xxx maybe replace w/ acquire remembering if FL_IF
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

  // create a fake process per CPU
  // so each CPU always has a tss and a gdt
  for(p = &proc[0]; p < &proc[NCPU]; p++){
    p->state = IDLEPROC;
    p->kstack = cpus[p-proc].mpstack;
    p->pid = p - proc;
  }

  // fix process 0 so that copyproc() will work
  p = &proc[0];
  p->sz = 4 * PAGE;
  p->mem = kalloc(p->sz);
  memset(p->mem, 0, p->sz);
  p->kstack = kalloc(KSTACKSIZE);
  p->tf = (struct trapframe *) (p->kstack + KSTACKSIZE - sizeof(struct trapframe));
  memset(p->tf, 0, sizeof(struct trapframe));
  p->tf->es = p->tf->ds = p->tf->ss = (SEG_UDATA << 3) | 3;
  p->tf->cs = (SEG_UCODE << 3) | 3;
  p->tf->eflags = FL_IF;
  setupsegs(p);

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

  p = copyproc(&proc[0]);
  
  //load_icode(p, _binary_usertests_start, (uint) _binary_usertests_size);
  //load_icode(p, _binary_userfs_start, (uint) _binary_userfs_size);
  load_icode(p, _binary_init_start, (uint) _binary_init_size);
  p->state = RUNNABLE;

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

  setupsegs(&proc[cpu()]);

  cpuid(0, 0, 0, 0, 0);	// memory barrier
  cpus[cpu()].booted = 1;

  // Enable interrupts on this processor.
  cpus[cpu()].nlock--;
  sti();

  scheduler();
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
  p->tf->esp = p->sz;

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
