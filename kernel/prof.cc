#include "types.h"
#include "kernel.hh"
#include "spinlock.h"
#include "condvar.h"
#include "fs.h"
#include "file.hh"
#include "prof.hh"
#include "bits.hh"
#include "amd64.h"

extern profctr_t sprof[];
extern profctr_t eprof[];
int profenable;

void
profreset(void)
{
  profctr_t *p = sprof;
  for (; p != eprof; p++) {
    memset(p->rec, 0, sizeof(p->rec));
  }
}

static void
profsum(struct profctr *ctr, u64 *tot, u64 *cnt)
{
  for (int i = 0; i < NCPU; i++) {
    *tot += ctr->rec[i].tot;
    *cnt += ctr->rec[i].cnt;
  }
}

void
profdump(void)
{
  profctr_t *p = sprof;
  for (; p != eprof; p++) {
    u64 tot = 0, cnt = 0;
    profsum(p, &tot, &cnt);
    if (cnt)
      cprintf("%s %lu\n", p->name, tot/cnt);
  }

  sampdump();
}
