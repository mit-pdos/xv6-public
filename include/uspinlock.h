#pragma once
#include "amd64.h"

struct uspinlock {
  volatile u32 locked;       // Is the lock held?
};

static void inline __attribute__((always_inline))
acquire(struct uspinlock *lk)
{
  while(xchg32(&lk->locked, 1) != 0)
    ;
}

static void inline __attribute__((always_inline))
release(struct uspinlock *lk)
{
  xchg32(&lk->locked, 0);
}

static int inline
tryacquire(struct uspinlock *lk)
{
  return xchg32(&lk->locked, 1) == 0;
}

static void inline
initlock(struct uspinlock *lk)
{
  lk->locked = 0;
}
