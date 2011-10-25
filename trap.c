#include "types.h"
#include "param.h"
#include "mmu.h"
#include "kernel.h"
#include "x86.h"
#include "cpu.h"

u64 ticks __mpalign__;

struct segdesc  __attribute__((aligned(16))) bootgdt[NSEGS] = {
  // null
  [0]=SEGDESC(0, 0, 0),
  // 32-bit kernel code
  [1]=SEGDESC(0, 0xfffff, SEG_R|SEG_CODE|SEG_S|SEG_DPL(0)|SEG_P|SEG_D|SEG_G),
  // 64-bit kernel code
  [2]=SEGDESC(0, 0, SEG_R|SEG_CODE|SEG_S|SEG_DPL(0)|SEG_P|SEG_L|SEG_G),
  // kernel data
  [3]=SEGDESC(0, 0xfffff, SEG_W|SEG_S|SEG_DPL(0)|SEG_P|SEG_D|SEG_G),
  // 64-bit user code
  [6]=SEGDESC(0, 0, SEG_R|SEG_CODE|SEG_S|SEG_DPL(3)|SEG_P|SEG_L|SEG_G),  
  // 64-bit user data
  [7]=SEGDESC(0, 0xfffff, SEG_W|SEG_S|SEG_DPL(3)|SEG_P|SEG_D|SEG_G)
};

struct intdesc idt[256] __attribute__((aligned(16)));

// boot.S
extern u64 trapentry[];

void
trap(struct trapframe *tf)
{
  cprintf("rip %lx rsp %lx\n", tf->rip, tf->rsp);
  panic("trap");
}

void
inittrap(void)
{
  uint64 entry;
  uint32 bits;
  int i;
  
  bits = INT_P | SEG_INTR64;  // present, interrupt gate
  for(i=0; i<256; i++) {
    entry = trapentry[i];
    idt[i] = INTDESC(KCSEG, entry, bits);
  }
}

void
initseg(void)
{
  volatile struct desctr dtr;
  struct cpu *c;

  dtr.limit = sizeof(idt) - 1;
  dtr.base = (u64)idt;
  lidt((void *)&dtr.limit);

  // TLS might not be ready
  c = &cpus[cpunum()];
  // Load per-CPU GDT
  memmove(c->gdt, bootgdt, sizeof(bootgdt));
  dtr.limit = sizeof(c->gdt) - 1;
  dtr.base = (u64)c->gdt;
  lgdt((void *)&dtr.limit);
}
