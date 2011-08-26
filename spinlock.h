// Mutual exclusion lock for short code fragments
struct spinlock {
  uint locked;       // Is the lock held?
  
  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
  uint pcs[10];      // The call stack (an array of program counters)
                     // that locked the lock.
};

// Lock that maybe held across sleeps
struct sleeplock {
  uint locked;       // Is the lock held?
};

