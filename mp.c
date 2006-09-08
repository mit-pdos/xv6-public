#include "types.h"
#include "mp.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "traps.h"
#include "mmu.h"
#include "proc.h"

static char *buses[] = {
  "CBUSI ",
  "CBUSII",
  "EISA  ",
  "FUTURE",
  "INTERN",
  "ISA   ",
  "MBI   ",
  "MBII  ",
  "MCA   ",
  "MPI   ",
  "MPSA  ",
  "NUBUS ",
  "PCI   ",
  "PCMCIA",
  "TC    ",
  "VL    ",
  "VME   ",
  "XPRESS",
  0,
};

struct cpu cpus[NCPU];
int ismp;
int ncpu;
uchar ioapic_id;

static struct cpu *bcpu;
static struct mp *mp;  // The MP floating point structure

static struct mp*
mp_scan(uchar *addr, int len)
{
  uchar *e, *p, sum;
  int i;

  e = addr+len;
  for(p = addr; p < e; p += sizeof(struct mp)){
    if(memcmp(p, "_MP_", 4))
      continue;
    sum = 0;
    for(i = 0; i < sizeof(struct mp); i++)
      sum += p[i];
    if(sum == 0)
      return (struct mp*)p;
  }
  return 0;
}

// Search for the MP Floating Pointer Structure, which according to the
// spec is in one of the following three locations:
// 1) in the first KB of the EBDA;
// 2) in the last KB of system base memory;
// 3) in the BIOS ROM between 0xE0000 and 0xFFFFF.
static struct mp*
mp_search(void)
{
  uchar *bda;
  uint p;
  struct mp *mp;

  bda = (uchar*) 0x400;
  if((p = (bda[0x0F]<<8)|bda[0x0E])){
    if((mp = mp_scan((uchar*) p, 1024)))
      return mp;
  }
  else{
    p = ((bda[0x14]<<8)|bda[0x13])*1024;
    if((mp = mp_scan((uchar*)p-1024, 1024)))
      return mp;
  }
  return mp_scan((uchar*)0xF0000, 0x10000);
}

// Search for an MP configuration table. For now,
// don't accept the default configurations (physaddr == 0).
// Check for correct signature, calculate the checksum and,
// if correct, check the version.
// To do: check extended table checksum.
static int
mp_detect(void)
{
  struct mpctb *pcmp;
  uchar *p, sum;
  uint length;

  if((mp = mp_search()) == 0 || mp->physaddr == 0)
    return 1;

  pcmp = (struct mpctb*) mp->physaddr;
  if(memcmp(pcmp, "PCMP", 4))
    return 2;

  length = pcmp->length;
  sum = 0;
  for(p = (uchar*)pcmp; length; length--)
    sum += *p++;

  if(sum || (pcmp->version != 1 && pcmp->version != 4))
    return 3;

  return 0;
}

void
mp_init(void)
{
  int r;
  uchar *p, *e;
  struct mpctb *mpctb;
  struct mppe *proc;
  struct mpbe *bus;
  struct mpioapic *ioapic;
  struct mpie *intr;
  int i;
  uchar byte;

  ncpu = 0;
  if((r = mp_detect()) != 0) {
    return;
  }

  ismp = 1;

  // Run through the table saving information needed for starting
  // application processors and initialising any I/O APICs. The table
  // is guaranteed to be in order such that only one pass is necessary.

  mpctb = (struct mpctb*) mp->physaddr;
  lapicaddr = (uint*) mpctb->lapicaddr;
  p = ((uchar*)mpctb)+sizeof(struct mpctb);
  e = ((uchar*)mpctb)+mpctb->length;

  while(p < e) {
    switch(*p){
    case MPPROCESSOR:
      proc = (struct mppe*) p;
      cpus[ncpu].apicid = proc->apicid;
      if(proc->flags & MPBP) {
        bcpu = &cpus[ncpu];
      }
      ncpu++;
      p += sizeof(struct mppe);
      continue;
    case MPBUS:
      bus = (struct mpbe*) p;
      for(i = 0; buses[i]; i++){
        if(strncmp(buses[i], bus->string, sizeof(bus->string)) == 0)
          break;
      }
      p += sizeof(struct mpbe);
      continue;
    case MPIOAPIC:
      ioapic = (struct mpioapic*) p;
      ioapic_id = ioapic->apicno;
      p += sizeof(struct mpioapic);
      continue;
    case MPIOINTR:
      intr = (struct mpie*) p;
      p += sizeof(struct mpie);
      continue;
    default:
      cprintf("mp_init: unknown PCMP type 0x%x (e-p 0x%x)\n", *p, e-p);
      while(p < e){
        cprintf("%uX ", *p);
        p++;
      }
      break;
    }
  }

  if(mp->imcrp) {
    // It appears that Bochs doesn't support IMCR, so code won't run.
    outb(0x22, 0x70);   // Select IMCR
    byte = inb(0x23);   // Current contents
    byte |= 0x01;       // Mask external INTR
    outb(0x23, byte);   // Disconnect 8259s/NMI
  }
}


int
mp_bcpu(void)
{
  if(ismp)
    return bcpu-cpus;
  return 0;
}

extern void mpmain(void);

// Write bootstrap code to unused memory at 0x7000.
#define APBOOTCODE 0x7000

void
mp_startthem(void)
{
  extern uchar _binary_bootother_start[], _binary_bootother_size[];
  extern int main();
  int c;

  memmove((void*) APBOOTCODE,_binary_bootother_start,
          (uint) _binary_bootother_size);

  for(c = 0; c < ncpu; c++){
    // Our current cpu has already started.
    if(c == cpu())
      continue;

    // Set target %esp
    *(uint*)(APBOOTCODE-4) = (uint) (cpus[c].mpstack) + MPSTACK;

    // Set target %eip
    *(uint*)(APBOOTCODE-8) = (uint)mpmain;

    // Go!
    lapic_startap(cpus[c].apicid, (uint) APBOOTCODE);

    // Wait for cpu to get through bootstrap.
    while(cpus[c].booted == 0)
      ;
  }
}
