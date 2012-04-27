#pragma once
#include "lockstat.h"

#if LOCKSTAT
extern klockstat klockstat_lazy;
#endif

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

#if LOCKSTAT
  struct klockstat *stat;
#endif

  // Create a spinlock that will later be initialized by initlock.
  // XXX(austin) Remove this and initlock in favor of the other ctor.
  constexpr spinlock()
    : locked(0)
#if SPINLOCK_DEBUG
    , name(nullptr), cpu(nullptr), pcs{}
#endif
#if LOCKSTAT
    , stat(nullptr)
#endif
  { }

  // Create a spinlock.  This is constexpr, so it can be used for
  // global spinlocks without incurring a static constructor.
  constexpr spinlock(const char *name, bool lockstat = false)
    : locked(0)
#if SPINLOCK_DEBUG
    , name(name), cpu(nullptr), pcs{}
#endif
#if LOCKSTAT
    , stat(lockstat ? nullptr : &klockstat_lazy)
#endif
  { }
};

#if SPINLOCK_DEBUG
#define lockname(s) ((s)->name ?: "null")
#else
#define lockname(s) ("unknown")
#endif
