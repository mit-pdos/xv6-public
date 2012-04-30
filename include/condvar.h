#pragma once

#include "queue.h"

struct condvar {
  struct spinlock lock;
  LIST_HEAD(waiters, proc) waiters;

#ifdef __cplusplus
  // Create a condvar that will later be initialized by initcondvar.
  // XXX(austin) Remove this and initcondvar.
  constexpr condvar()
    : lock(), waiters{} { }

  constexpr
  condvar(const char *name)
    : lock(name, LOCKSTAT_CONDVAR), waiters{} { }
#endif // __cplusplus
};
