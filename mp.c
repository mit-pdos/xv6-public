#include "types.h"
#include "mp.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "traps.h"
#include "mmu.h"
#include "proc.h"

static char* buses[] = {
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

#define APBOOTCODE 0x7000 // XXX hack

static struct MP* mp;  // The MP floating point structure
struct cpu cpus[NCPU];
int ncpu;
static struct cpu *bcpu;

static struct MP*
mp_scan(uint8_t *addr, int len)
{
  uint8_t *e, *p, sum;
  int i;

  cprintf("scanning: 0x%x\n", (uint32_t)addr);
  e = addr+len;
  for(p = addr; p < e; p += sizeof(struct MP)){
    if(memcmp(p, "_MP_", 4))
      continue;
    sum = 0;
    for(i = 0; i < sizeof(struct MP); i++)
      sum += p[i];
    if(sum == 0)
      return (struct MP *)p;
  }
  return 0;
}

static struct MP*
mp_search(void)
{
  uint8_t *bda;
  uint32_t p;
  struct MP *mp;

  /*
   * Search for the MP Floating Pointer Structure, which according to the
   * spec is in one of the following three locations:
   * 1) in the first KB of the EBDA;
   * 2) in the last KB of system base memory;
   * 3) in the BIOS ROM between 0xE0000 and 0xFFFFF.
   */
  bda = (uint8_t*) 0x400;
  if((p = (bda[0x0F]<<8)|bda[0x0E])){
    if((mp = mp_scan((uint8_t*) p, 1024)))
      return mp;
  }
  else{
    p = ((bda[0x14]<<8)|bda[0x13])*1024;
    if((mp = mp_scan((uint8_t*)p-1024, 1024)))
      return mp;
  }
  return mp_scan((uint8_t*)0xF0000, 0x10000);
}

static int 
mp_detect(void)
{
  struct MPCTB *pcmp;
  uint8_t *p, sum;
  uint32_t length;

  /*
   * Search for an MP configuration table. For now,
   * don't accept the default configurations (physaddr == 0).
   * Check for correct signature, calculate the checksum and,
   * if correct, check the version.
   * To do: check extended table checksum.
   */
  if((mp = mp_search()) == 0 || mp->physaddr == 0)
    return 1;

  pcmp = (struct MPCTB *) mp->physaddr;
  if(memcmp(pcmp, "PCMP", 4))
    return 2;

  length = pcmp->length;
  sum = 0;
  for(p = (uint8_t*)pcmp; length; length--)
    sum += *p++;

  if(sum || (pcmp->version != 1 && pcmp->version != 4))
    return 3;

  cprintf("Mp spec rev #: %x imcrp 0x%x\n", mp->specrev, mp->imcrp);
  return 0;
}

void
mp_init(void)
{ 
  int r;
  uint8_t *p, *e;
  struct MPCTB *mpctb;
  struct MPPE *proc;
  struct MPBE *bus;
  int i;

  ncpu = 0;
  if ((r = mp_detect()) != 0) return;

  cprintf ("This computer is a multiprocessor!\n");

  /*
   * Run through the table saving information needed for starting
   * application processors and initialising any I/O APICs. The table
   * is guaranteed to be in order such that only one pass is necessary.
   */
  mpctb = (struct MPCTB *) mp->physaddr;
  lapicaddr = (uint32_t *) mpctb->lapicaddr;
  cprintf("apicaddr: %x\n", lapicaddr);
  p = ((uint8_t*)mpctb)+sizeof(struct MPCTB);
  e = ((uint8_t*)mpctb)+mpctb->length;

  while(p < e) {
    switch(*p){
    case MPPROCESSOR:
      proc = (struct MPPE *) p;
      cpus[ncpu].apicid = proc->apicid;
      cprintf("a processor %x\n", cpus[ncpu].apicid);
      if (proc->flags & MPBP) {
	bcpu = &cpus[ncpu];
      }
      ncpu++;
      p += sizeof(struct MPPE);
      continue;
    case MPBUS:
      bus = (struct MPBE *) p;
      for(i = 0; buses[i]; i++){
	if(strncmp(buses[i], bus->string, sizeof(bus->string)) == 0)
	  break;
      }
      cprintf("a bus %d\n", i);
      p += sizeof(struct MPBE);
      continue;
    case MPIOAPIC:
      cprintf("an I/O APIC\n");
      p += sizeof(struct MPIOAPIC);
      continue;
    case MPIOINTR:
      cprintf("an I/O intr\n");
      p += sizeof(struct MPIE);
      continue;
    default:
      cprintf("mpinit: unknown PCMP type 0x%x (e-p 0x%x)\n", *p, e-p);
      while(p < e){
	cprintf("%uX ", *p);
	p++;
      }
      break;
    }
  }
  
  cprintf("ncpu: %d boot %d\n", ncpu, bcpu-cpus);
}

int
mp_bcpu(void)
{
  return bcpu-cpus;
}

extern void mpmain(void);

void
mp_startthem(void)
{
  extern uint8_t _binary_bootother_start[], _binary_bootother_size[];
  extern int main();
  int c;

  memmove((void *) APBOOTCODE,_binary_bootother_start, 
	  (uint32_t) _binary_bootother_size);

  for(c = 0; c < ncpu; c++){
    if (c == cpu()) continue;
    cprintf ("starting processor %d\n", c);
    *(uint *)(APBOOTCODE-4) = (uint) (cpus[c].mpstack) + MPSTACK; // tell it what to use for %esp
    *(uint *)(APBOOTCODE-8) = (uint)mpmain; // tell it where to jump to
    lapic_startap(cpus[c].apicid, (uint32_t) APBOOTCODE);
  }
}
