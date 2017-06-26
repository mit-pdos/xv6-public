// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
  addr_t pcs[10];      // The call stack (an array of program counters)
                     // that locked the lock.
};

