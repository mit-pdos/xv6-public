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

struct pmu {
  void (*config)(u64 ctr, u64 sel, u64 val);  
  u64 cntval_bits;
};
struct pmu pmu;

//
// AMD stuff
//
static void
amdconfig(u64 ctr, u64 sel, u64 val)
{
  writemsr(MSR_AMD_PERF_SEL0 | ctr, 0);
  writemsr(MSR_AMD_PERF_CNT0 | ctr, val);
  writemsr(MSR_AMD_PERF_SEL0 | ctr, sel);
}

struct pmu amdpmu = {
  .config = amdconfig,
  .cntval_bits = 48,
};

//
// Intel stuff
//
static void
intelconfig(u64 ctr, u64 sel, u64 val)
{
  writemsr(MSR_INTEL_PERF_SEL0 | ctr, 0);
  writemsr(MSR_INTEL_PERF_CNT0 | ctr, val);
  writemsr(MSR_INTEL_PERF_SEL0 | ctr, sel);
}

// XXX
struct pmu intelpmu = {
  .config = intelconfig,
  .cntval_bits = 48,
};

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

void
profstart(void)
{
  u64 ctr = 0;
  u64 sel = 0;
  u64 val = -100000;

  sel = 0UL << 32 |
    1 << 24 | 
    1 << 22 | 
    1 << 20 |
    1 << 17 | 
    1 << 16 | 
    0x00 << 8 | 
    0x76;

  pushcli();
  pmu.config(ctr, sel, val);
  popcli();
}

int
profintr(void)
{
  lapicpc(0);
  // Only level-triggered interrupts require an lapiceoi.
  return 1;
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
    if (!strcmp(s, "AuthenticAMD"))
      pmu = amdpmu;
    else if (!strcmp(s, "GenuineIntel"))
      pmu = intelpmu;
    else
      panic("Unknown Manufacturer");
  }

  // enable RDPMC at CPL > 0
  u64 cr4 = rcr4();
  lcr4(cr4 | CR4_PCE);
}
