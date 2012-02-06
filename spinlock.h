#pragma once

// Mutual exclusion lock.
struct spinlock {
  u32 locked;       // Is the lock held?

#if SPINLOCK_DEBUG
  // For debugging:
  const char *name;  // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
  uptr pcs[10];      // The call stack (an array of program counters)
                     // that locked the lock.
#endif
};

#if SPINLOCK_DEBUG
#define lockname(s) ((s)->name ?: "null")
#else
#define lockname(s) ("unknown")
#endif
