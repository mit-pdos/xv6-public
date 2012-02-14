extern "C" {
#include "types.h"
#include "kernel.h"
}

void *
operator new(unsigned long nbytes)
{
  return kmalloc(nbytes);
}

void
operator delete(void *p)
{
  kmfree(p);
}

extern "C" void
__cxa_pure_virtual(void)
{
  panic("__cxa_pure_virtual");
}
