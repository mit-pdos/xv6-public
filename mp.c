// Multiprocessor bootstrap.
// Search memory for MP description structures.
// http://developer.intel.com/design/pentium/datashts/24201606.pdf

#include "types.h"
#include "param.h"
#include "x86.h"
#include "mp.h"
#include "kernel.h"
#include "cpu.h"

struct cpu cpus[NCPU];
static struct cpu *bcpu __mpalign__;
int ismp __mpalign__;
int ncpu __mpalign__;
u8 ioapicid __mpalign__;

int
mpbcpu(void)
{
  return bcpu-cpus;
}

static u8
sum(u8 *addr, int len)
{
  int i, sum;
  
  sum = 0;
  for(i=0; i<len; i++)
    sum += addr[i];
  return sum;
}

// Look for an MP structure in the len bytes at addr.
static struct mp*
mpsearch1(u8 *addr, int len)
{
  u8 *e, *p;

  addr = p2v((uptr) addr);
  e = addr+len;
  for(p = addr; p < e; p += sizeof(struct mp))
    if(memcmp(p, "_MP_", 4) == 0 && sum(p, sizeof(struct mp)) == 0)
      return (struct mp*)p;
  return 0;
}

// Search for the MP Floating Pointer Structure, which according to the
// spec is in one of the following three locations:
// 1) in the first KB of the EBDA;
// 2) in the last KB of system base memory;
// 3) in the BIOS ROM between 0xE0000 and 0xFFFFF.
static struct mp*
mpsearch(void)
{
  u8 *bda;
  paddr p;
  struct mp *mp;

  bda = (u8*)0x400;
  if((p = ((bda[0x0F]<<8)|bda[0x0E]) << 4)){
    if((mp = mpsearch1((u8*)p, 1024)))
      return mp;
  } else {
    p = ((bda[0x14]<<8)|bda[0x13])*1024;
    if((mp = mpsearch1((u8*)p-1024, 1024)))
      return mp;
  }
  return mpsearch1((u8*)0xF0000, 0x10000);
}

// Search for an MP configuration table.  For now,
// don't accept the default configurations (physaddr == 0).
// Check for correct signature, calculate the checksum and,
// if correct, check the version.
// To do: check extended table checksum.
static struct mpconf*
mpconfig(struct mp **pmp)
{
  struct mpconf *conf;
  struct mp *mp;

  if((mp = mpsearch()) == 0 || mp->physaddr == 0)
    return 0;
  conf = (struct mpconf*) p2v((uptr) mp->physaddr);
  if(memcmp(conf, "PCMP", 4) != 0)
    return 0;
  if(conf->version != 1 && conf->version != 4)
    return 0;
  if(sum((u8*)conf, conf->length) != 0)
    return 0;
  *pmp = mp;
  return conf;
}

void
initmp(void)
{
  u8 *p, *e;
  struct mp *mp;
  struct mpconf *conf;
  struct mpproc *proc;
  struct mpioapic *ioapic;

  bcpu = &cpus[0];
  if((conf = mpconfig(&mp)) == 0)
    return;
  ismp = 1;

  for(p=(u8*)(conf+1), e=(u8*)conf+conf->length; p<e; ){
    switch(*p){
    case MPPROC:
      proc = (struct mpproc*)p;
      if(ncpu != proc->apicid){
        cprintf("mpinit: ncpu=%d apicid=%d\n", ncpu, proc->apicid);
        ismp = 0;
      }
      if(proc->flags & MPBOOT)
        bcpu = &cpus[ncpu];
      cpus[ncpu].id = ncpu;
      ncpu++;
      p += sizeof(struct mpproc);
      continue;
    case MPIOAPIC:
      ioapic = (struct mpioapic*)p;
      ioapicid = ioapic->apicno;
      p += sizeof(struct mpioapic);
      continue;
    case MPBUS:
    case MPIOINTR:
    case MPLINTR:
      p += 8;
      continue;
    default:
      cprintf("mpinit: unknown config type %x\n", *p);
      ismp = 0;
    }
  }
  if(!ismp){
    // Didn't like what we found; fall back to no MP.
    ncpu = 1;
    ioapicid = 0;
    return;
  }

  if(mp->imcrp){
    // Bochs doesn't support IMCR, so this doesn't run on Bochs.
    // But it would on real hardware.
    outb(0x22, 0x70);   // Select IMCR
    outb(0x23, inb(0x23) | 1);  // Mask external interrupts.
  }
}
