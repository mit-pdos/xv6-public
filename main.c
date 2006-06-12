#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"

char junk1[20000];
char junk2[20000] = { 1 };

main()
{
  struct proc *p;

  cprintf("\nxV6\n\n");

  // initialize physical memory allocator
  kinit();

  // create fake process zero
  p = &proc[0];
  p->state = WAITING;
  p->sz = PAGE;
  p->mem = kalloc(p->sz);
  memset(p->mem, 0, p->sz);
  p->kstack = kalloc(KSTACKSIZE);
  p->tf = (struct Trapframe *) (p->kstack + KSTACKSIZE - sizeof(struct Trapframe));
  memset(p->tf, 0, sizeof(struct Trapframe));
  p->tf->tf_es = p->tf->tf_ds = p->tf->tf_ss = (SEG_UDATA << 3) | 3;
  p->tf->tf_cs = (SEG_UCODE << 3) | 3;
  p->tf->tf_eflags = FL_IF;
  setupsegs(p);

  p = newproc(&proc[0]);
  // xxx copy instructions to p->mem
  p->tf->tf_eip = 0;
  p->tf->tf_esp = p->sz;

  swtch(&proc[0]);
}
