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

static const u64 debug_sel = 
  0UL << 32 |
  1 << 24 | 
  1 << 22 | 
  1 << 20 |
  1 << 17 | 
  1 << 16 | 
  0x00 << 8 | 
  0x76;

static const u64 debug_cnt = 100000;

extern profctr_t sprof[];
extern profctr_t eprof[];
int profenable;

struct pmu {
  void (*config)(u64 ctr, u64 sel, u64 val);  
  u64 cntval_bits;
};
struct pmu pmu;

struct pmuevent {
  u64 rip;
};

struct pmulog {
  u64 head;
  u64 tail;
  u64 size;
  u64 try;
  struct pmuevent *event;
  __padout__;
} __mpalign__;

struct pmulog pmulog[NCPU] __mpalign__;

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

  cprintf("pmu\n");
  for (int c = 0; c < NCPU; c++) {
    struct pmulog *l = &pmulog[c];    
    for (u64 i = l->tail; i < l->head; i++)
      cprintf(" %lx\n", l->event[i % l->size].rip);
  }
}

void
profstart(void)
{
  pushcli();
  pmu.config(0, debug_sel, -debug_cnt);
  popcli();
}

static int
proflog(struct trapframe *tf)
{
  struct pmulog *l;
  l = &pmulog[cpunum()];
  l->try++;

  if ((l->head - l->tail) == l->size)
    return 0;

  l->event[l->head % l->size].rip = tf->rip;
  l->head++;
  return 1;
}

int
profintr(struct trapframe *tf)
{
  // Acquire locks that are we only acquire during NMI.
  // NMIs are disabled until the next iret.

  // Linux unmasks LAPIC.PC after every interrupt (perf_event.c)
  lapicpc(0);
  // Only level-triggered interrupts require an lapiceoi.

  u64 cnt = rdpmc(0);
  if (cnt & (1ULL << (pmu.cntval_bits - 1)))
    return 0;

  if (proflog(tf))
    pmu.config(0, debug_sel, -debug_cnt);

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
  
  void *p = kalloc();
  if (p == NULL)
    panic("initprof: kalloc");
  pmulog[cpunum()].event = p;
  pmulog[cpunum()].size = PGSIZE / sizeof(struct pmuevent);
}
