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

struct jmpbuf {
  // saved registers for kernel context switches
  // don't need to save all the fs etc. registers because
  // they are constant across kernel contexts
  // save all the regular registers so we don't care which are caller save
  // don't save eax because that's the return register
  // layout known to swtch.S
  int jb_ebx;
  int jb_ecx;
  int jb_edx;
  int jb_esi;
  int jb_edi;
  int jb_esp;
  int jb_ebp;
  int jb_eip;
};

enum proc_state { UNUSED, EMBRYO, WAITING, RUNNABLE, RUNNING, ZOMBIE };

struct proc{
  char *mem; // start of process's physical memory
  unsigned sz; // total size of mem, including kernel stack
  char *kstack; // kernel stack, separate from mem so it doesn't move
  enum proc_state state;
  enum proc_state newstate; // desired state after swtch()
  int pid;
  int ppid;
  void *chan; // sleep
  int killed;
  struct fd *fds[NOFILE];

  struct Taskstate ts;  // only to give cpu address of kernel stack
  struct Segdesc gdt[NSEGS];
  struct Pseudodesc gdt_pd;
  unsigned esp; // kernel stack pointer
  unsigned ebp; // kernel frame pointer

  struct jmpbuf jmpbuf;

  struct Trapframe *tf; // points into kstack, used to find user regs
};

extern struct proc proc[];
extern struct proc *curproc[NCPU];  // can be NULL if no proc running.
  // XXX move curproc into cpu structure?

#define MPSTACK 512

struct cpu {
  uint8_t apicid;       // Local APIC ID
  struct jmpbuf jmpbuf;
  char mpstack[MPSTACK]; // per-cpu start-up stack, only used to get into main()
  struct proc *lastproc;  // last proc scheduled on this cpu (never NULL)
};

extern struct cpu cpus[NCPU];
extern int ncpu;
