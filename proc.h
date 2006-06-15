/*
 * p->mem:
 *   text
 *   original data and bss
 *   fixed-size stack
 *   expandable heap
 */

/*
 * segments in proc->gdt
 */
#define SEG_KCODE 1 // kernel code
#define SEG_KDATA 2 // kernel data+stack
#define SEG_UCODE 3
#define SEG_UDATA 4
#define SEG_TSS 5   // this process's task state
#define NSEGS 6

struct proc{
  char *mem; // start of process's physical memory
  unsigned sz; // total size of mem, including kernel stack
  char *kstack; // kernel stack, separate from mem so it doesn't move
  enum { UNUSED, RUNNABLE, WAITING, ZOMBIE } state;
  int pid;
  int ppid;
  void *chan; // sleep

  struct Taskstate ts;  // only to give cpu address of kernel stack
  struct Segdesc gdt[NSEGS];
  struct Pseudodesc gdt_pd;
  unsigned esp; // kernel stack pointer
  unsigned ebp; // kernel frame pointer

  struct Trapframe *tf; // points into kstack, used to find user regs
};

extern struct proc proc[];
extern struct proc *curproc;
