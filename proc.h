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
// which are caller save, but not the return register %eax.
// (Not saving %eax just simplifies the switching code.)
// The layout of context must match code in swtch.S.
struct context {
  int eip;
  int esp;
  int ebx;
  int ecx;
  int edx;
  int esi;
  int edi;
  int ebp;
};

enum proc_state { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Per-process state
struct proc {
  char *mem;                // Start of process memory (kernel address)
  uint sz;                  // Size of process memory (bytes)
  char *kstack;             // Bottom of kernel stack for this process
  enum proc_state state;    // Process state
  int pid;                  // Process ID
  struct proc *parent;      // Parent process
  void *chan;               // If non-zero, sleeping on chan
  int killed;               // If non-zero, have been killed
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;        // Current directory
  struct context context;   // Switch here to run process
  struct trapframe *tf;     // Trap frame for current interrupt
  char name[16];            // Process name (debugging)
};

// Process memory is laid out contiguously, low addresses first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap

// Per-CPU state
struct cpu {
  uchar apicid;               // Local APIC ID
  struct proc *curproc;       // Process currently running.
  struct context context;     // Switch here to enter scheduler
  struct taskstate ts;        // Used by x86 to find stack for interrupt
  struct segdesc gdt[NSEGS];  // x86 global descriptor table
  volatile uint booted;        // Has the CPU started?
  int ncli;                   // Depth of pushcli nesting.
  int intena;                 // Were interrupts enabled before pushcli? 
};

extern struct cpu cpus[NCPU];
extern int ncpu;

// "cp" is a short alias for curproc().
// It gets used enough to make this worthwhile.
#define cp curproc()
