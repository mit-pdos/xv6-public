extern "C" {
#include "types.h"
#include "kernel.h"
}

#include "cpputil.hh"

void *
operator new(unsigned long nbytes)
{
  return kmalloc(nbytes);
}

void *
operator new(unsigned long nbytes, void *buf)
{
  return buf;
}

void
operator delete(void *p)
{
  kmfree(p);
}

void
__cxa_pure_virtual(void)
{
  panic("__cxa_pure_virtual");
}
