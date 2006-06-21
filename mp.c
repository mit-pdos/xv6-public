#include "types.h"
#include "mp.h"
#include "defs.h"
#include "memlayout.h"

static struct _MP_* _mp_;  /* The MP floating point structure */
static int ncpu;

static struct _MP_*
mp_scan(uint8_t *addr, int len)
{
  uint8_t *e, *p, sum;
  int i;

  cprintf("scanning: 0x%x\n", (uint32_t)addr);
  e = addr+len;
  for(p = addr; p < e; p += sizeof(struct _MP_)){
    if(memcmp(p, "_MP_", 4))
      continue;
    sum = 0;
    for(i = 0; i < sizeof(struct _MP_); i++)
      sum += p[i];
    if(sum == 0)
      return (struct _MP_ *)p;
  }
  return 0;
}

static struct _MP_*
mp_search(void)
{
  uint8_t *bda;
  uint32_t p;
  struct _MP_ *mp;

  /*
   * Search for the MP Floating Pointer Structure, which according to the
   * spec is in one of the following three locations:
   * 1) in the first KB of the EBDA;
   * 2) in the last KB of system base memory;
   * 3) in the BIOS ROM between 0xE0000 and 0xFFFFF.
   */
  bda = KADDR(0x400);
  if((p = (bda[0x0F]<<8)|bda[0x0E])){
    if((mp = mp_scan(KADDR(p), 1024)))
      return mp;
  }
  else{
    p = ((bda[0x14]<<8)|bda[0x13])*1024;
    if((mp = mp_scan(KADDR(p-1024), 1024)))
      return mp;
  }
  return mp_scan(KADDR(0xF0000), 0x10000);
}

static int 
mp_detect(void)
{
  struct PCMP *pcmp;
  uint8_t *p, sum;
  uint32_t length;

  /*
   * Search for an MP configuration table. For now,
   * don't accept the default configurations (physaddr == 0).
   * Check for correct signature, calculate the checksum and,
   * if correct, check the version.
   * To do: check extended table checksum.
   */
  if((_mp_ = mp_search()) == 0 || _mp_->physaddr == 0)
    return 1;

  pcmp = KADDR(_mp_->physaddr);
  if(memcmp(pcmp, "PCMP", 4))
    return 2;

  length = pcmp->length;
  sum = 0;
  for(p = (uint8_t*)pcmp; length; length--)
    sum += *p++;

  if(sum || (pcmp->version != 1 && pcmp->version != 4))
    return 3;

  cprintf("MP spec rev #: %x\n", _mp_->specrev);
  return 0;
}

void
mpinit()
{ 
  int r;
  uint8_t *p, *e;
  struct PCMP *pcmp;

  ncpu = 0;
  if ((r = mp_detect()) != 0) return;
  cprintf ("This computer is multiprocessor!\n");

  /*
   * Run through the table saving information needed for starting
   * application processors and initialising any I/O APICs. The table
   * is guaranteed to be in order such that only one pass is necessary.
   */
  pcmp = KADDR(_mp_->physaddr);
  p = ((uint8_t*)pcmp)+sizeof(struct PCMP);
  e = ((uint8_t*)pcmp)+pcmp->length;

  while(p < e) {
    switch(*p){
    case PcmpPROCESSOR:
      cprintf("a processor\n");
      ncpu++;
      p += sizeof(struct PCMPprocessor);
      continue;
    case PcmpBUS:
      cprintf("a bus\n");
      p += sizeof(struct PCMPbus);
      continue;
    case PcmpIOAPIC:
      cprintf("an IO APIC\n");
      p += sizeof(struct PCMPioapic);
      continue;
    case PcmpIOINTR:
      cprintf("an IO interrupt assignment\n");
      p += sizeof(struct PCMPintr);
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

  cprintf("ncpu: %d\n", ncpu);
}
