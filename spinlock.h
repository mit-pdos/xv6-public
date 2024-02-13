#ifndef SPINLOCK_H_5FAE6694_CAAF_11EE_AA29_90B11C0C0FF8
#define SPINLOCK_H_5FAE6694_CAAF_11EE_AA29_90B11C0C0FF8
// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
  uint pcs[10];      // The call stack (an array of program counters)
                     // that locked the lock.
};

#endif
