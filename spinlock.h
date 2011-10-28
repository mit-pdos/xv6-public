#pragma once

#define	SPINLOCK_DEBUG		1

// Mutual exclusion lock.
struct spinlock {
  u32 locked;       // Is the lock held?

#if SPINLOCK_DEBUG
  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
  uptr pcs[10];      // The call stack (an array of program counters)
                     // that locked the lock.
#endif
};

static inline const char *lockname(struct spinlock *s)
{
#if SPINLOCK_DEBUG
  return s->name ?: "null";
#else
  return "unknown";
#endif
}
