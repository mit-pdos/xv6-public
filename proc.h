// Segments in proc->gdt
#define SEG_KCODE 1  // kernel code
#define SEG_KDATA 2  // kernel data+stack
#define SEG_UCODE 3
#define SEG_UDATA 4
#define SEG_TSS   5  // this process's task state
#define NSEGS     6

// Saved registers for kernel context switches.
// Don't need to save all the %fs etc. segment registers,
// because they are constant across kernel contexts.
// Save all the regular registers so we don't need to care
// which are caller save.
// Don't save %eax, because that's the return register.
// The layout of jmpbuf is known to setjmp.S.
struct jmpbuf {
  int ebx;
  int ecx;
  int edx;
  int esi;
  int edi;
  int esp;
  int ebp;
  int eip;
};

enum proc_state { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Per-process state
struct proc {
  char *mem;                // Start of process memory (kernel address)
  uint sz;                  // Size of process memory (bytes)
  char *kstack;             // Bottom of kernel stack for this process
  enum proc_state state;    // Process state
  int pid;                  // Process ID
  int ppid;                 // Parent pid
  void *chan;               // If non-zero, sleeping on chan
  int killed;               // If non-zero, have been killed
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;        // Current directory
  struct jmpbuf jmpbuf;     // Jump here to run process
  struct trapframe *tf;     // Trap frame for current interrupt
};

// Process memory is laid out contiguously:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap

extern struct proc proc[];
extern struct proc *curproc[NCPU];  // Current (running) process per CPU

#define MPSTACK 512

// Per-CPU state
struct cpu {
  uchar apicid;               // Local APIC ID
  struct jmpbuf jmpbuf;       // Jump here to enter scheduler
  struct taskstate ts;        // Used by x86 to find stack for interrupt
  struct segdesc gdt[NSEGS];  // x86 global descriptor table
  char mpstack[MPSTACK];      // Per-CPU startup stack
  volatile int booted;        // Has the CPU started?
  int nlock;                  // Number of locks currently held
};

extern struct cpu cpus[NCPU];
extern int ncpu;
