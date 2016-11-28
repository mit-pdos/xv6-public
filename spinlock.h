#include <stdatomic.h>

// Mutual exclusion lock.
struct spinlock {
  atomic_uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  _Atomic(struct cpu *) cpu;   // The cpu holding the lock.
  uint pcs[10];      // The call stack (an array of program counters)
                     // that locked the lock.
};

