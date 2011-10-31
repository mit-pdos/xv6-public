#include "spinlock.h"

// Saved registers for kernel context switches.
// (also implicitly defined in swtch.S)
struct context {
  u64 r15;
  u64 r14;
  u64 r13;
  u64 r12;
  u64 rbp;
  u64 rbx;
  u64 rip;
} __attribute__((packed));

// Per-process, per-stack meta data for mtrace
#if MTRACE
#define MTRACE_NSTACKS 16
#define MTRACE_TAGSHIFT 28
#if NCPU > 16
#error Oops -- decrease MTRACE_TAGSHIFT
#endif
struct mtrace_stacks {
  int curr;
  unsigned long tag[MTRACE_NSTACKS];
};
#endif

enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Per-process state
struct proc {
  struct vmap *vmap;           // va -> vma
  uptr brk;                    // Top of heap
  char *kstack;                // Bottom of kernel stack for this process
  enum procstate state;        // Process state
  volatile int pid;            // Process ID
  struct proc *parent;         // Parent process
  struct trapframe *tf;        // Trap frame for current syscall
  struct context *context;     // swtch() here to run process
  struct condvar *oncv;        // Where it is sleeping, for kill()
  struct proc *cv_next;        // Linked list of processes waiting for oncv
  int killed;                  // If non-zero, have been killed
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;           // Current directory
  char name[16];               // Process name (debugging)
  unsigned long long tsc;
  unsigned long long curcycles;
  unsigned cpuid;
  struct spinlock lock;
  SLIST_HEAD(childlist, proc) childq;
  SLIST_ENTRY(proc) child_next;
  struct condvar cv;
  u64 epoch;
  u64 rcu_read_depth;
  char lockname[16];
  int on_runq;
  int cpu_pin;
#if MTRACE
  struct mtrace_stacks mtrace_stacks;
#endif
  struct runq *runq;
  STAILQ_ENTRY(proc) runqlink;
};

extern struct ns *nspid;
