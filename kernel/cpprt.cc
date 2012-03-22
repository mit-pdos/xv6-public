#include "types.h"
#include "kernel.hh"
#include "cpputil.hh"
#include "spinlock.h"
#include "amd64.h"

void *
operator new[](unsigned long nbytes)
{
  u64 *x = (u64*) kmalloc(nbytes + sizeof(u64), "array");
  *x = nbytes + sizeof(u64);
  return x+1;
}

void
operator delete[](void *p)
{
  u64 *x = (u64*) p;
  x--;
  kmfree(x, *x);
}

void
__cxa_pure_virtual(void)
{
  panic("__cxa_pure_virtual");
}

int
__cxa_guard_acquire(s64 *guard)
{
  volatile u8 *x = (u8*) guard;
  volatile u32 *l = (u32*) (x+4);

  pushcli();
  while (xchg32(l, 1) != 0)
    ; /* spin */

  if (*x) {
    xchg32(l, 0);
    popcli();
    return 0;
  }
  return 1;
}

void
__cxa_guard_release(s64 *guard)
{
  volatile u8 *x = (u8*) guard;
  volatile u32 *l = (u32*) (x+4);

  *x = 1;
  __sync_synchronize();
  xchg32(l, 0);
  popcli();
}

int
__cxa_atexit(void (*f)(void*), void *p, void *d)
{
  return 0;
}

void *__dso_handle;
std::ostream std::cout;

namespace std {

template<>
u128
atomic<u128>::load(memory_order __m) const
{
  __sync_synchronize();
  u128 v = _M_i;
  __sync_synchronize();

  return v;
}

template<>
bool
atomic<u128>::compare_exchange_weak(u128 &__i1, u128 i2, memory_order __m)
{
  // XXX no __sync_val_compare_and_swap for u128
  u128 o = __i1;
  bool ok = __sync_bool_compare_and_swap(&_M_i, o, i2);
  if (!ok)
    __i1 = _M_i;
  return ok;
}

};
