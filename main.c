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
extern int acpu;
extern uint8_t _binary_user1_start[], _binary_user1_size[];
extern uint8_t _binary_usertests_start[], _binary_usertests_size[];
extern uint8_t _binary_userfs_start[], _binary_userfs_size[];

extern int use_console_lock;

int
main()
{
  struct proc *p;

  if (acpu) {
    cpus[cpu()].clis = 1;
    cprintf("an application processor\n");
    idtinit(); // CPU's idt
    lapic_init(cpu());
    lapic_timerinit();
    lapic_enableintr();
    sti();
    scheduler();
  }
  acpu = 1;

  // clear BSS
  memset(edata, 0, end - edata);

  mp_init(); // collect info about this machine

  use_console_lock = 1;

  cpus[cpu()].clis = 1; // cpu starts as if we had called cli()

  lapic_init(mp_bcpu());

  cprintf("\nxV6\n\n");

  pic_init(); // initialize PIC
  kinit(); // physical memory allocator
  tvinit(); // trap vectors
  idtinit(); // CPU's idt

  // create fake process zero
  p = &proc[0];
  memset(p, 0, sizeof *p);
  p->state = WAITING;
  p->sz = 4 * PAGE;
  p->mem = kalloc(p->sz);
  memset(p->mem, 0, p->sz);
  p->kstack = kalloc(KSTACKSIZE);
  p->tf = (struct Trapframe *) (p->kstack + KSTACKSIZE - sizeof(struct Trapframe));
  memset(p->tf, 0, sizeof(struct Trapframe));
  p->tf->tf_es = p->tf->tf_ds = p->tf->tf_ss = (SEG_UDATA << 3) | 3;
  p->tf->tf_cs = (SEG_UCODE << 3) | 3;
  p->tf->tf_eflags = FL_IF;
  p->pid = 0;
  p->ppid = 0;
  setupsegs(p);

  mp_startthem();

  // turn on timer and enable interrupts on the local APIC
  lapic_timerinit();
  lapic_enableintr();

  // init disk device
  //ide_init(); 

  // become interruptable
  sti();

  p = newproc();
  
  load_icode(p, _binary_usertests_start, (unsigned) _binary_usertests_size);
  //load_icode(p, _binary_userfs_start, (unsigned) _binary_userfs_size);
  p->state = RUNNABLE;
  cprintf("loaded userfs\n");

  scheduler();

  return 0;
}

void
load_icode(struct proc *p, uint8_t *binary, unsigned size)
{
  int i;
  struct Elf *elf;
  struct Proghdr *ph;

  // Check magic number on binary
  elf = (struct Elf*) binary;
  cprintf("elf %x magic %x\n", elf, elf->e_magic);
  if (elf->e_magic != ELF_MAGIC)
    panic("load_icode: not an ELF binary");

  p->tf->tf_eip = elf->e_entry;
  p->tf->tf_esp = p->sz;

  // Map and load segments as directed.
  ph = (struct Proghdr*) (binary + elf->e_phoff);
  for (i = 0; i < elf->e_phnum; i++, ph++) {
    if (ph->p_type != ELF_PROG_LOAD)
      continue;
    cprintf("va %x memsz %d\n", ph->p_va, ph->p_memsz);
    if (ph->p_va + ph->p_memsz < ph->p_va)
      panic("load_icode: overflow in elf header segment");
    if (ph->p_va + ph->p_memsz >= p->sz)
      panic("load_icode: icode wants to be above UTOP");

    // Load/clear the segment
    memcpy(p->mem + ph->p_va, binary + ph->p_offset, ph->p_filesz);
    memset(p->mem + ph->p_va + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
  }
}
