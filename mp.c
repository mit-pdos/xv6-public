#include "types.h"
#include "mp.h"
#include "defs.h"
#include "memlayout.h"
#include "param.h"
#include "x86.h"
#include "traps.h"
#include "mmu.h"

/* 
 * Credit: Plan 9 sources, Intel MP spec, and Cliff Frey
 */

enum {					/* Local APIC registers */
  LAPIC_ID  = 0x0020,	/* ID */
  LAPIC_VER = 0x0030,	/* Version */
  LAPIC_TPR = 0x0080,	/* Task Priority */
  LAPIC_APR = 0x0090,	/* Arbitration Priority */
  LAPIC_PPR = 0x00A0,	/* Processor Priority */
  LAPIC_EOI = 0x00B0,	/* EOI */
  LAPIC_LDR = 0x00D0,	/* Logical Destination */
  LAPIC_DFR = 0x00E0,	/* Destination Format */
  LAPIC_SVR = 0x00F0,	/* Spurious Interrupt Vector */
  LAPIC_ISR = 0x0100,	/* Interrupt Status (8 registers) */
  LAPIC_TMR = 0x0180,	/* Trigger Mode (8 registers) */
  LAPIC_IRR = 0x0200,	/* Interrupt Request (8 registers) */
  LAPIC_ESR = 0x0280,	/* Error Status */
  LAPIC_ICRLO = 0x0300,	/* Interrupt Command */
  LAPIC_ICRHI = 0x0310,	/* Interrupt Command [63:32] */
  LAPIC_TIMER = 0x0320,	/* Local Vector Table 0 (TIMER) */
  LAPIC_PCINT = 0x0340,	/* Performance Counter LVT */
  LAPIC_LINT0 = 0x0350,	/* Local Vector Table 1 (LINT0) */
  LAPIC_LINT1 = 0x0360,	/* Local Vector Table 2 (LINT1) */
  LAPIC_ERROR = 0x0370,	/* Local Vector Table 3 (ERROR) */
  LAPIC_TICR = 0x0380,	/* Timer Initial Count */
  LAPIC_TCCR = 0x0390,	/* Timer Current Count */
  LAPIC_TDCR = 0x03E0,	/* Timer Divide Configuration */
};

enum {					/* LAPIC_SVR */
  LAPIC_ENABLE	= 0x00000100,	/* Unit Enable */
  LAPIC_FOCUS	= 0x00000200,	/* Focus Processor Checking Disable */
};

enum {					/* LAPIC_ICRLO */
					/* [14] IPI Trigger Mode Level (RW) */
  LAPIC_DEASSERT = 0x00000000,	/* Deassert level-sensitive interrupt */
  LAPIC_ASSERT	= 0x00004000,	/* Assert level-sensitive interrupt */

  /* [17:16] Remote Read Status */
  LAPIC_INVALID	= 0x00000000,	/* Invalid */
  LAPIC_WAIT	= 0x00010000,	/* In-Progress */
  LAPIC_VALID	= 0x00020000,	/* Valid */

  /* [19:18] Destination Shorthand */
  LAPIC_FIELD	= 0x00000000,	/* No shorthand */
  LAPIC_SELF	= 0x00040000,	/* Self is single destination */
  LAPIC_ALLINC	= 0x00080000,	/* All including self */
  LAPIC_ALLEXC	= 0x000C0000,	/* All Excluding self */
};

enum {					/* LAPIC_ESR */
  LAPIC_SENDCS	= 0x00000001,	/* Send CS Error */
  LAPIC_RCVCS	= 0x00000002,	/* Receive CS Error */
  LAPIC_SENDACCEPT = 0x00000004,	/* Send Accept Error */
  LAPIC_RCVACCEPT = 0x00000008,	/* Receive Accept Error */
  LAPIC_SENDVECTOR = 0x00000020,	/* Send Illegal Vector */
  LAPIC_RCVVECTOR = 0x00000040,	/* Receive Illegal Vector */
  LAPIC_REGISTER = 0x00000080,	/* Illegal Register Address */
};

enum {					/* LAPIC_TIMER */
					/* [17] Timer Mode (RW) */
  LAPIC_ONESHOT	= 0x00000000,	/* One-shot */
  LAPIC_PERIODIC = 0x00020000,	/* Periodic */

  /* [19:18] Timer Base (RW) */
  LAPIC_CLKIN	= 0x00000000,	/* use CLKIN as input */
  LAPIC_TMBASE	= 0x00040000,	/* use TMBASE */
  LAPIC_DIVIDER	= 0x00080000,	/* use output of the divider */
};

enum {					/* LAPIC_TDCR */
  LAPIC_X2 = 0x00000000,	/* divide by 2 */
  LAPIC_X4 = 0x00000001,	/* divide by 4 */
  LAPIC_X8 = 0x00000002,	/* divide by 8 */
  LAPIC_X16 = 0x00000003,	/* divide by 16 */
  LAPIC_X32 = 0x00000008,	/* divide by 32 */
  LAPIC_X64 = 0x00000009,	/* divide by 64 */
  LAPIC_X128 = 0x0000000A,	/* divide by 128 */
  LAPIC_X1 = 0x0000000B,	/* divide by 1 */
};

#define APBOOTCODE 0x7000 // XXX hack
#define MPSTACK 512

static struct MP* mp;  // The MP floating point structure
static uint32_t *lapicaddr;
static struct cpu {
  uint8_t apicid;       // Local APIC ID
  int lintr[2];		// Local APIC
  char mpstack[MPSTACK]; // per-cpu start-up stack, only used to get into main()
} cpus[NCPU];
static int ncpu;
static struct cpu *bcpu;

static int
lapic_read(int r)
{
  return *(lapicaddr+(r/sizeof(*lapicaddr)));
}

static void
lapic_write(int r, int data)
{
  *(lapicaddr+(r/sizeof(*lapicaddr))) = data;
}


void
lapic_timerinit()
{
  cprintf("%d: init timer\n", cpu());
  lapic_write(LAPIC_TDCR, LAPIC_X1);
  lapic_write(LAPIC_TIMER, LAPIC_CLKIN | LAPIC_PERIODIC | (IRQ_OFFSET + IRQ_TIMER));
  lapic_write(LAPIC_TCCR, 1000000);
  lapic_write(LAPIC_TICR, 1000000);
}

void
lapic_timerintr()
{
  cprintf("%d: timer interrupt!\n", cpu());
  lapic_write (LAPIC_EOI, 0);
}

void
lapic_init(int c)
{
  uint32_t r, lvt;

  cprintf("lapic_init %d\n", c);

  irq_setmask_8259A(0xFFFF);

  lapic_write(LAPIC_DFR, 0xFFFFFFFF);
  r = (lapic_read(LAPIC_ID)>>24) & 0xFF;
  lapic_write(LAPIC_LDR, (1<<r)<<24);
  lapic_write(LAPIC_TPR, 0xFF);
  lapic_write(LAPIC_SVR, LAPIC_ENABLE|(IRQ_OFFSET+IRQ_SPURIOUS));

  /*
   * Set the local interrupts. It's likely these should just be
   * masked off for SMP mode as some Pentium Pros have problems if
   * LINT[01] are set to ExtINT.
   * Acknowledge any outstanding interrupts.
   */
  lapic_write(LAPIC_LINT0, cpus[c].lintr[0]);
  lapic_write(LAPIC_LINT1, cpus[c].lintr[1]);
  lapic_write(LAPIC_EOI, 0);

  lvt = (lapic_read(LAPIC_VER)>>16) & 0xFF;
  if(lvt >= 4)
    lapic_write(LAPIC_PCINT, APIC_IMASK);
  lapic_write(LAPIC_ERROR, IRQ_OFFSET+IRQ_ERROR);
  lapic_write(LAPIC_ESR, 0);
  lapic_read(LAPIC_ESR);

  /*
   * Issue an INIT Level De-Assert to synchronise arbitration ID's.
   */
  lapic_write(LAPIC_ICRHI, 0);
  lapic_write(LAPIC_ICRLO, LAPIC_ALLINC|APIC_LEVEL|LAPIC_DEASSERT|APIC_INIT);
  while(lapic_read(LAPIC_ICRLO) & APIC_DELIVS)
    ;

  cprintf("Done init of an apic\n");
}

void
lapic_enableintr(void) 
{
  lapic_write(LAPIC_TPR, 0);
}

void
lapic_disableintr(void) 
{
  lapic_write(LAPIC_TPR, 0xFF);
}

int
cpu(void)
{
  return (lapic_read(LAPIC_ID)>>24) & 0xFF;
}

static void
lapic_startap(struct cpu *c, int v)
{
  int crhi, i;
  volatile int j = 0;

  crhi = c->apicid<<24;
  lapic_write(LAPIC_ICRHI, crhi);
  lapic_write(LAPIC_ICRLO, LAPIC_FIELD|APIC_LEVEL|LAPIC_ASSERT|APIC_INIT);

  while (j++ < 10000) {;}
  lapic_write(LAPIC_ICRLO, LAPIC_FIELD|APIC_LEVEL|LAPIC_DEASSERT|APIC_INIT);

  while (j++ < 1000000) {;}

  // in p9 code, this was i < 2, which is what the spec says on page B-3
  for(i = 0; i < 1; i++){
    lapic_write(LAPIC_ICRHI, crhi);
    lapic_write(LAPIC_ICRLO, LAPIC_FIELD|APIC_EDGE|APIC_STARTUP|(v/PGSIZE));
    while (j++ < 100000) {;}
  }
}

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

  cprintf("Mp spec rev #: %x\n", mp->specrev);
  return 0;
}

int
mp_isbcpu()
{
  if (bcpu == 0) return 1;
  else return 0;
}

void
mp_init()
{ 
  int r;
  uint8_t *p, *e;
  struct MPCTB *mpctb;
  struct MPPE *proc;
  int c;
  extern int main();

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
      cpus[ncpu].lintr[0] = APIC_IMASK;
      cpus[ncpu].lintr[1] = APIC_IMASK;
      cprintf("a processor %x\n", cpus[ncpu].apicid);
      if (proc->flags & MPBP) {
	bcpu = &cpus[ncpu];
      }
      ncpu++;
      p += sizeof(struct MPPE);
      continue;
    case MPBUS:
      p += sizeof(struct MPBE);
      continue;
    case MPIOAPIC:
      cprintf("an I/O APIC\n");
      p += sizeof(struct MPIOAPIC);
      continue;
    case MPIOINTR:
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
  
  lapic_init(bcpu-cpus);
  cprintf("ncpu: %d boot %d\n", ncpu, bcpu-cpus);

  extern uint8_t _binary_bootother_start[], _binary_bootother_size[];
  memmove((void *) APBOOTCODE,_binary_bootother_start, 
	  (uint32_t) _binary_bootother_size);

  acquire_spinlock(&kernel_lock);
  for(c = 0; c < ncpu; c++){
    if (cpus+c == bcpu) continue;
    cprintf ("starting processor %d\n", c);
    release_grant_spinlock(&kernel_lock, c);
    *(unsigned *)(APBOOTCODE-4) = (unsigned) (cpus[c].mpstack) + MPSTACK; // tell it what to use for %esp
    *(unsigned *)(APBOOTCODE-8) = (unsigned)&main; // tell it where to jump to
    lapic_startap(cpus + c, (uint32_t) APBOOTCODE);
    acquire_spinlock(&kernel_lock);
    cprintf ("done starting processor %d\n", c);
  }
}
