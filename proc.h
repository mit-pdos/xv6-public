#include "spinlock.h"

// Segments in proc->gdt.
// Also known to bootasm.S and trapasm.S
#define SEG_KCODE 1  // kernel code
#define SEG_KDATA 2  // kernel data+stack
#define SEG_KCPU  3  // kernel per-cpu data
#define SEG_UCODE 4  // user code
#define SEG_UDATA 5  // user data+stack
#define SEG_TSS   6  // this process's task state
#define NSEGS     7

//PAGEBREAK: 17
// Saved registers for kernel context switches.
// Don't need to save all the segment registers (%cs, etc),
// because they are constant across kernel contexts.
// Don't need to save %eax, %ecx, %edx, because the
// x86 convention is that the caller has saved them.
// Contexts are stored at the bottom of the stack they
// describe; the stack pointer is the address of the context.
// The layout of the context matches the layout of the stack in swtch.S
// at the "Switch stacks" comment. Switch doesn't save eip explicitly,
// but it is on the stack and allocproc() manipulates it.
struct context {
  uint edi;
  uint esi;
  uint ebx;
  uint ebp;
  uint eip;
};

enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// A memory object (physical pages or inode).
enum vmntype { EAGER, ONDEMAND};  
struct vmnode {
  uint npages;
  char *page[128];
  uint ref;
  enum vmntype type;
  struct inode *ip;
  uint offset;
  uint sz;
};

// A mapping of a chunk of an address space to
// a specific memory object.
enum vmatype { PRIVATE, COW};
struct vma {
  uint va_start;               // start of mapping
  uint va_end;                 // one past the last byte
  enum vmatype va_type;
  struct vmnode *n;
  struct spinlock lock;        // serialize fault/unmap
  char lockname[16];
};

#define TREE

// An address space: a set of vmas plus h/w page table.
// The elements of e[] are not ordered by address.
struct vmap {
#ifdef TREE
  struct node* root;
#else
  struct vma* e[16];
#endif
  struct spinlock lock;        // serialize map/lookup/unmap
  uint ref;
  uint alloc;
  pde_t *pgdir;                // Page table
  char lockname[16];
};

// Per-process state
struct proc {
  struct vmap *vmap;           // va -> vma
  uint brk;                    // Top of heap
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
  uint epoch;
  uint rcu_read_depth;
  char lockname[16];
  int on_runq;
  int cpu_pin;
};

// Process memory is laid out contiguously, low addresses first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap

// Per-CPU state
struct cpu {
  uchar id;                    // Local APIC ID; index into cpus[] below
  struct context *scheduler;   // swtch() here to enter scheduler
  struct taskstate ts;         // Used by x86 to find stack for interrupt
  struct segdesc gdt[NSEGS];   // x86 global descriptor table
  volatile uint booted;        // Has the CPU started?
  int ncli;                    // Depth of pushcli nesting.
  int intena;                  // Were interrupts enabled before pushcli?
  int last_rcu_gc_ticks;
  
  // Cpu-local storage variables; see below
  struct cpu *cpu;
  struct proc *proc;           // The currently-running process.
  struct kmem *kmem;           // The per-core memory table
} __attribute__ ((aligned (CACHELINE)));

struct condtab {
  char name[MAXNAME];
  struct spinlock lock;
  struct condvar condtab[NPROC];
} __attribute__ ((aligned (CACHELINE)));

extern struct cpu cpus[NCPU];
extern struct condtab condtabs[NCPU];
extern int ncpu;
extern struct ns *nspid;

// Per-CPU variables, holding pointers to the
// current cpu and to the current process.
// The asm suffix tells gcc to use "%gs:0" to refer to cpu
// and "%gs:4" to refer to proc.  seginit sets up the
// %gs segment register so that %gs refers to the memory
// holding those two variables in the local cpu's struct cpu.
// This is similar to how thread-local variables are implemented
// in thread libraries such as Linux pthreads.
extern struct cpu *cpu __asm("%gs:0");       // &cpus[cpunum()].cpu
extern struct proc *proc __asm("%gs:4");     // cpus[cpunum()].proc
extern struct kmem *kmem __asm("%gs:8"); // &cpu[cpunum()].kmem
