#include "types.h"
#include "kernel.hh"
#include "cpputil.hh"

void *
operator new[](unsigned long nbytes)
{
  u64 *x = (u64*) kmalloc(nbytes + sizeof(u64));
  *x = nbytes;
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
