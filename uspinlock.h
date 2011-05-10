#pragma once
#include "x86.h"

struct uspinlock {
  volatile uint locked;       // Is the lock held?
};

static void inline __attribute__((always_inline))
acquire(struct uspinlock *lk)
{
  while(xchg(&lk->locked, 1) != 0)
    ;
}

static void inline __attribute__((always_inline))
release(struct uspinlock *lk)
{
  xchg(&lk->locked, 0);
}

