#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "traps.h"
#include "syscall.h"

extern char edata[], end[];

char buf[512];

int
main()
{
  struct proc *p;
  int i;
  
  // clear BSS
  memset(edata, 0, end - edata);

  cprintf("\nxV6\n\n");

  mpinit(); // multiprocessor
  kinit(); // physical memory allocator
  tinit(); // traps and interrupts
  pic_init();

  // create fake process zero
  p = &proc[0];
  curproc = p;
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
  p->pid = 0;
  p->ppid = 0;
  setupsegs(p);

  // turn on interrupts
  write_eflags(read_eflags() | FL_IF);
  irq_setmask_8259A(0);

#if 0
  ide_read(0, buf, 1);
  cprintf("sec0.0 %x\n", buf[0] & 0xff);
#endif

#if 0
  p = newproc();

  i = 0;
  p->mem[i++] = 0x90; // nop 
  p->mem[i++] = 0xb8; // mov ..., %eax
  p->mem[i++] = SYS_fork;
  p->mem[i++] = 0;
  p->mem[i++] = 0;
  p->mem[i++] = 0;
  p->mem[i++] = 0xcd; // int
  p->mem[i++] = T_SYSCALL;
  p->mem[i++] = 0xb8; // mov ..., %eax
  p->mem[i++] = SYS_wait;
  p->mem[i++] = 0;
  p->mem[i++] = 0;
  p->mem[i++] = 0;
  p->mem[i++] = 0xcd; // int
  p->mem[i++] = T_SYSCALL;
  p->mem[i++] = 0xb8; // mov ..., %eax
  p->mem[i++] = SYS_exit;
  p->mem[i++] = 0;
  p->mem[i++] = 0;
  p->mem[i++] = 0;
  p->mem[i++] = 0xcd; // int
  p->mem[i++] = T_SYSCALL;
  p->tf->tf_eip = 0;
  p->tf->tf_esp = p->sz;
#endif

  swtch();

  return 0;
}
