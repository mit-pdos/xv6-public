#include "types.h"
#include "stat.h"
#include "user.h"
#include "lib.h"
#include "percpu.hh"

#define WQCHUNKSZ 8192
#define WQBLOCKSZ 128
static_assert(WQCHUNKSZ%WQBLOCKSZ == 0, "Bad sizes");

struct wqblock {
  wqblock* next;
};
  
percpu<wqblock*> block;

static bool
refill(void)
{
  long r = map(0, WQCHUNKSZ);
  if (r < 0)
    return false;

  for (uptr p = r; p < r+WQCHUNKSZ; p += WQBLOCKSZ) {
    wqblock* n = (wqblock*)p;
    n->next = *block;
    *block = n;
  }
  return true;
}

void*
wqalloc(unsigned long nbytes)
{
  assert(nbytes <= WQBLOCKSZ);

  if (*block == nullptr)
    if (!refill())
      return nullptr;

  wqblock *r = *block;
  *block = r->next;
  return r;
}

void
wqfree(void *ptr)
{
  wqblock* b = (wqblock*) ptr;
  b->next = *block;
  *block = b;
}
