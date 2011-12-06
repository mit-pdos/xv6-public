#include "types.h"
#include "kernel.h"
#include "e820.h"

// Make is easier to copy from dmesg
#define E820_LINUX(start, end, t) \
  { .addr = start, .size = end - start, .type = t }

#if defined(HW_qemu)
  // For qemu -m 512
struct e820entry e820[NE820] = {
  E820_LINUX(0x0000000000000000ull, 0x000000000009f400ull, E820_RAM),
  E820_LINUX(0x000000000009f400ull, 0x00000000000a0000ull, E820_RESERVED),
  E820_LINUX(0x00000000000f0000ull, 0x0000000000100000ull, E820_RESERVED),
  E820_LINUX(0x0000000000100000ull, 0x000000001fffd000ull, E820_RAM),
  E820_LINUX(0x000000001fffd000ull, 0x0000000020000000ull, E820_RESERVED),
  E820_LINUX(0x00000000fffc0000ull, 0x0000000100000000ull, E820_RESERVED)
};
#elif defined(HW_josmp)
#elif defined(HW_ud0)
#endif

static struct e820entry *
e820search(paddr pa)
{
  struct e820entry *e;
  struct e820entry *q;

  q = &e820[NE820];
  for (e = &e820[0]; e < q; e++) {
    if (e->type == E820_RAM && (e->addr <= pa) && ((e->addr + e->size) > pa))
      return e;
  }

  return NULL;
}

u64
e820bytes(void)
{
  u64 tot = 0;
  for (int i = 0; i < NE820; i++) {
    if (e820[i].type == E820_RAM)
      tot += e820[i].size;
  }
  return tot;
}

u64
e820size(void *va)
{
  struct e820entry *e;
  paddr pa = v2p(va);

  e = e820search(pa);
  if (e == NULL)
    return -1;

  return (e->addr + e->size) - pa;
}

void *
e820next(void *va, u64 inc)
{
  struct e820entry *e;
  paddr pa = v2p(va);

  e = e820search(pa);
  if (e == NULL)
    return (void *)-1;

  pa += inc;
  if (pa < (e->addr + e->size))
    return p2v(pa);

  for (e = e + 1; e < &e820[NE820]; e++) {
    if (e->type == E820_RAM)
      return p2v(e->addr);
  }

  return (void *)-1;
}
