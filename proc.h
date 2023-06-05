#ifndef XV6_PROC_H
#define XV6_PROC_H

#include "types.h"
#include "mmu.h"
#include "param.h"
#include "vfs_file.h"

struct cgroup;

// Per-CPU state
struct cpu {
  uchar apicid;                // Local APIC ID
  struct context *scheduler;   // swtch() here to enter scheduler
  struct taskstate ts;         // Used by x86 to find stack for interrupt
  struct segdesc gdt[NSEGS];   // x86 global descriptor table
  volatile uint started;       // Has the CPU started?
  int ncli;                    // Depth of pushcli nesting.
  int intena;                  // Were interrupts enabled before pushcli?
  struct proc *proc;           // The process running on this cpu or null
};

extern struct cpu cpus[NCPU];
extern int ncpu;

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

struct pid_entry {
  struct pid_ns* pid_ns;
  int pid;
};


// Per-process state
struct proc {
  uint sz;                     // Size of process memory (bytes)
  pde_t* pgdir;                // Page table
  char *kstack;                // Bottom of kernel stack for this process
  enum procstate state;        // Process state
  /* int pid;                     // Process ID */
  int ns_pid;
  struct pid_entry pids[4];
  struct proc *parent;         // Parent process
  struct trapframe *tf;        // Trap frame for current syscall
  struct context *context;     // swtch() here to run process
  void *chan;                  // If non-zero, sleeping on chan
  int killed;                  // If non-zero, have been killed
  struct vfs_file *ofile[NOFILE];  // Open files
  struct vfs_inode *cwd;           // Current directory
  struct mount *cwdmount;      // Mount in which current directory lies
  char name[16];               // Process name (debugging)
  struct nsproxy *nsproxy;     // Namespace proxy object
  struct pid_ns *child_pid_ns; // PID namespace for child procs
  int status;                  // Process exit status
  char cwdp[MAX_PATH_LENGTH];  // Current directory path.
  struct cgroup * cgroup;      // The process control group.
  unsigned int cpu_time;       // Process cpu time.
  unsigned int cpu_period_time;// Cpu time in microseconds in the last accounting frame.
  unsigned int cpu_percent;   // Cpu usage percentage in the last accounting frame.
  unsigned int cpu_account_frame; // The cpu account frame.
};

/**
 * Returns the pid of the given proc, using the current
 * process namespace.
 */
int proc_pid(struct proc * proc);

/**
 * Locks the process table.
 */
void proc_lock();

/**
 * Unlocks the process table.
 */
void proc_unlock();

/**
 * @brief Getter for a cgroup associated with this process
 *
 * @return Pointer to a cgroup this process is associated with
 */
struct cgroup *proc_get_cgroup(void);

// Process memory is laid out contiguously, low addresses first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap

#endif /* XV6_PROC_H */
