#include "types.h"
#include "amd64.h"
#include "cpu.hh"
#include "rnd.hh"

struct seed {
  u64 v;
} __mpalign__;
static struct seed seeds[NCPU] __mpalign__;

u64 
rnd(void)
{
  if (seeds[mycpu()->id].v == 0) {
    seeds[mycpu()->id].v = rdtsc();
  }
  seeds[mycpu()->id].v = 
    seeds[mycpu()->id].v * 6364136223846793005 + 1442695040888963407;
  return seeds[mycpu()->id].v;
}
