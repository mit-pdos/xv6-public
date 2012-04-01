#include "types.h"
#include "amd64.h"
#include "kernel.hh"
#include "percpu.hh"
#include "wq.hh"
#include "kalloc.hh"

extern "C" void clear_page(void*);

struct zallocator {
  run*   run;
  wframe frame;
};

percpu<zallocator> z_;

char*
zalloc(const char* name)
{
  char* p = kalloc(name);
  if (p)
    clear_page(p);
  return p;
}

void
zfree(char* p)
{
  kfree(p);
}

void
initz(void)
{
  for (int i = 0; i < NCPU; i++)
    z_[i].frame.clear();
}
