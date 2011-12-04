#include "param.h"
#include "types.h"
#include "spinlock.h"
#include "condvar.h"
#include "fs.h"
#include "file.h"
#include "prof.h"
#include "kernel.h"
#include "bits.h"
#include "amd64.h"

extern profctr_t sprof[];
extern profctr_t eprof[];

int profenable;

static void (*profconfig)(u64 ctr, u64 sel, u64 val);

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
}

static void
amdconfig(u64 ctr, u64 sel, u64 val)
{
  writemsr(MSR_AMD_PERF_SEL0 | ctr, 0);
  writemsr(MSR_AMD_PERF_CNT0 | ctr, val);
  writemsr(MSR_AMD_PERF_SEL0 | ctr, sel);
}

static void
intelconfig(u64 ctr, u64 sel, u64 val)
{
  writemsr(MSR_INTEL_PERF_SEL0 | ctr, 0);
  writemsr(MSR_INTEL_PERF_CNT0 | ctr, val);
  writemsr(MSR_INTEL_PERF_SEL0 | ctr, sel);
}

void
profstart(void)
{
  u64 ctr = 0;
  u64 sel = 0;
  u64 val = -1000;

  cprintf("profstart ...\n");

  sel = 0UL << 32 |
    1 << 24 | 
    1 << 22 | 
    1 << 20 |
    1 << 17 | 
    1 << 16 | 
    0x00 << 8 | 
    0x76;

  pushcli();
  profconfig(ctr, sel, val);
  popcli();

  cprintf("rdpmc %lx\n", rdpmc(0));
}

void
initprof(void)
{
  if (cpunum() == mpbcpu()) {
    u32 name[4];
    char *s = (char *)name;
    name[3] = 0;

    cpuid(0, 0, &name[0], &name[2], &name[1]);
    cprintf("%s\n", s);
    if (strcmp(s, "AuthenticAMD"))
      profconfig = amdconfig;
    else if (strcmp(s, "GenuineIntel"))
      profconfig = intelconfig;
    else
      panic("Unknown Manufacturer");
  }

  // enable RDPMC at CPL > 0
  u64 cr4 = rcr4();
  lcr4(cr4 | CR4_PCE);
}
