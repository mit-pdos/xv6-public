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
sampdump(void)
{
  for (int c = 0; c < NCPU; c++) {
    struct pmulog *l = &pmulog[c];    
    cprintf("%u samples:\n", c);
    for (u64 i = l->tail; i < l->head; i++)
      cprintf(" %lx\n", l->event[i % l->size].rip);
  }
}

void
sampstart(void)
{
  pushcli();
  pmu.config(0, debug_sel, -debug_cnt);
  popcli();
}

static int
samplog(struct trapframe *tf)
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
sampintr(struct trapframe *tf)
{
  // Acquire locks that we only acquire during NMI.
  // NMIs are disabled until the next iret.

  // Linux unmasks LAPIC.PC after every interrupt (perf_event.c)
  lapicpc(0);
  // Only level-triggered interrupts require an lapiceoi.

  u64 cnt = rdpmc(0);
  if (cnt & (1ULL << (pmu.cntval_bits - 1)))
    return 0;

  if (samplog(tf))
    pmu.config(0, debug_sel, -debug_cnt);

  return 1;
}

void
initsamp(void)
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
