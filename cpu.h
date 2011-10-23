// Per-CPU state
struct cpu {

  u8 id;                    // Local APIC ID; index into cpus[] below
  int ncli;                 // Depth of pushcli nesting.
  int intena;               // Were interrupts enabled before pushcli?
#if 0
  struct context *scheduler;   // swtch() here to enter scheduler
  struct taskstate ts;         // Used by x86 to find stack for interrupt
  struct segdesc gdt[NSEGS];   // x86 global descriptor table
  volatile uint booted;        // Has the CPU started?
  int last_rcu_gc_ticks;
  
  // Cpu-local storage variables; see below
  struct cpu *cpu;
  struct proc *proc;           // The currently-running process.
  struct kmem *kmem;           // The per-core memory table
#endif
} __mpalign__;


// Per-CPU variables, holding pointers to the
// current cpu and to the current process.
// The asm suffix tells gcc to use "%gs:0" to refer to cpu
// and "%gs:4" to refer to proc.  seginit sets up the
// %gs segment register so that %gs refers to the memory
// holding those two variables in the local cpu's struct cpu.
// This is similar to how thread-local variables are implemented
// in thread libraries such as Linux pthreads.
extern struct cpu *cpu __asm("%gs:0");       // &cpus[cpunum()].cpu
extern struct proc *proc __asm("%gs:8");     // cpus[cpunum()].proc
extern struct kmem *kmem __asm("%gs:16"); // &cpu[cpunum()].kmem
