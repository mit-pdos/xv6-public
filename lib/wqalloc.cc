#include "types.h"
#include "stat.h"
#include "user.h"
#include "lib.h"
#include "percpu.hh"

#include <sys/mman.h>

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
  void *r = mmap(0, WQCHUNKSZ, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
  if (r == MAP_FAILED)
    return false;

  for (char *p = (char*)r; p < (char*)r+WQCHUNKSZ; p += WQBLOCKSZ) {
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

void
wqallocinit(void)
{
  for (int i = 0; i < NCPU; i++)
    block[i] = nullptr;
}
