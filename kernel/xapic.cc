// The local APIC manages internal (non-I/O) interrupts.
// See Chapter 8 & Appendix C of Intel processor manual volume 3.

#include "types.h"
#include "amd64.h"
#include "kernel.hh"
#include "traps.h"
#include "bits.hh"
#include "cpu.hh"
#include "apic.hh"

// Local APIC registers, divided by 4 for use as uint[] indices.
#define ID      (0x0020/4)   // ID
#define VER     (0x0030/4)   // Version
#define TPR     (0x0080/4)   // Task Priority
#define EOI     (0x00B0/4)   // EOI
#define SVR     (0x00F0/4)   // Spurious Interrupt Vector
  #define ENABLE     0x00000100   // Unit Enable
#define ESR     (0x0280/4)   // Error Status
#define ICRLO   (0x0300/4)   // Interrupt Command
  #define INIT       0x00000500   // INIT/RESET
  #define STARTUP    0x00000600   // Startup IPI
  #define DELIVS     0x00001000   // Delivery status
  #define ASSERT     0x00004000   // Assert interrupt (vs deassert)
  #define DEASSERT   0x00000000
  #define LEVEL      0x00008000   // Level triggered
  #define BCAST      0x00080000   // Send to all APICs, including self.
// XXX(sbw) BUSY and DELIVS?
  #define BUSY       0x00001000
  #define FIXED      0x00000000
#define ICRHI   (0x0310/4)   // Interrupt Command [63:32]
#define TIMER   (0x0320/4)   // Local Vector Table 0 (TIMER)
  #define X1         0x0000000B   // divide counts by 1
  #define PERIODIC   0x00020000   // Periodic
#define PCINT   (0x0340/4)   // Performance Counter LVT
#define LINT0   (0x0350/4)   // Local Vector Table 1 (LINT0)
#define LINT1   (0x0360/4)   // Local Vector Table 2 (LINT1)
#define ERROR   (0x0370/4)   // Local Vector Table 3 (ERROR)
  #define MASKED     0x00010000   // Interrupt masked
  #define MT_NMI     0x00000400   // NMI message type
  #define MT_FIX     0x00000000   // Fixed message type
#define TICR    (0x0380/4)   // Timer Initial Count
#define TCCR    (0x0390/4)   // Timer Current Count
#define TDCR    (0x03E0/4)   // Timer Divide Configuration

#define IO_RTC  0x70

static volatile u32 *xapic = (u32 *)(KBASE + 0xfee00000);
static u64 xapichz;

static void
xapicw(int index, int value)
{
  xapic[index] = value;
  xapic[ID];  // wait for write to finish, by reading
}

static u32
xapicr(u32 off)
{
  return xapic[off];
}

static int
xapicwait()
{
  int i = 100000;
  while ((xapicr(ICRLO) & BUSY) != 0) {
    nop_pause();
    i--;
    if (i == 0) {
      cprintf("xapicwait: wedged?\n");
      return -1;
    }
  }
  return 0;
}

void
initxapic(void)
{
  u64 count;

  // Enable local APIC; set spurious interrupt vector.
  xapicw(SVR, ENABLE | (T_IRQ0 + IRQ_SPURIOUS));

  if (xapichz == 0) {
    // Measure the TICR frequency
    xapicw(TDCR, X1);    
    xapicw(TICR, 0xffffffff); 
    u64 ccr0 = xapicr(TCCR);
    microdelay(10 * 1000);    // 1/100th of a second
    u64 ccr1 = xapicr(TCCR);
    xapichz = 100 * (ccr0 - ccr1);
  }

  count = (QUANTUM*xapichz) / 1000;
  if (count > 0xffffffff)
    panic("initxapic: QUANTUM too large");

  // The timer repeatedly counts down at bus frequency
  // from xapic[TICR] and then issues an interrupt.  
  xapicw(TDCR, X1);
  xapicw(TIMER, PERIODIC | (T_IRQ0 + IRQ_TIMER));
  xapicw(TICR, count); 

  // Disable logical interrupt lines.
  xapicw(LINT0, MASKED);
  xapicw(LINT1, MASKED);

  // Disable performance counter overflow interrupts
  // on machines that provide that interrupt entry.
  if(((xapic[VER]>>16) & 0xFF) >= 4)
    xapicpc(0);

  // Map error interrupt to IRQ_ERROR.
  xapicw(ERROR, T_IRQ0 + IRQ_ERROR);

  // Clear error status register (requires back-to-back writes).
  xapicw(ESR, 0);
  xapicw(ESR, 0);

  // Ack any outstanding interrupts.
  xapicw(EOI, 0);

  // Send an Init Level De-Assert to synchronise arbitration ID's.
  xapicw(ICRHI, 0);
  xapicw(ICRLO, BCAST | INIT | LEVEL);
  while(xapic[ICRLO] & DELIVS)
    ;

  // Enable interrupts on the APIC (but not on the processor).
  xapicw(TPR, 0);
}

void
xapicpc(char mask)
{
  xapicw(PCINT, mask ? MASKED : MT_NMI);
}

hwid_t
xapicid(void)
{
  if (readrflags() & FL_IF) {
    cli();
    panic("cpunum() called from %p with interrupts enabled\n",
      __builtin_return_address(0));
  }

  if (xapic == nullptr)
    panic("xapicid");
  return HWID(xapic[ID]>>24);
}

// Acknowledge interrupt.
void
xapiceoi(void)
{
  if(xapic)
    xapicw(EOI, 0);
}

// Send IPI
void
xapic_ipi(hwid_t hwid, int ino)
{
  xapicw(ICRHI, hwid.num << 24);
  xapicw(ICRLO, FIXED | DEASSERT | ino);
  if (xapicwait() < 0)
    panic("xapic_ipi: xapicwait failure");
}

void
xapic_tlbflush(hwid_t hwid)
{
  xapic_ipi(hwid, T_TLBFLUSH);
}

void
xapic_sampconf(hwid_t hwid)
{
  xapic_ipi(hwid, T_SAMPCONF);
}

// Start additional processor running bootstrap code at addr.
// See Appendix B of MultiProcessor Specification.
void
xapicstartap(hwid hwid, u32 addr)
{
  int i;
  volatile u16 *wrv;

  // "The BSP must initialize CMOS shutdown code to 0AH
  // and the warm reset vector (DWORD based at 40:67) to point at
  // the AP startup code prior to the [universal startup algorithm]."
  outb(IO_RTC, 0xF);  // offset 0xF is shutdown code
  outb(IO_RTC+1, 0x0A);
  wrv = (u16*)(0x40<<4 | 0x67);  // Warm reset vector
  wrv[0] = 0;
  wrv[1] = addr >> 4;

  // "Universal startup algorithm."
  // Send INIT (level-triggered) interrupt to reset other CPU.
  

  xapicw(ICRHI, hwid.num<<24);
  // XXX(sbw) why hwid.num in ICRLO?
  xapicw(ICRLO, hwid.num | INIT | LEVEL | ASSERT);
  xapicwait();
  microdelay(10000);
  // XXX(sbw) why hwid.num in ICRLO?
  xapicw(ICRLO, hwid.num |INIT | LEVEL);
  xapicwait();
  microdelay(10000);    // should be 10ms, but too slow in Bochs!
  
  // Send startup IPI (twice!) to enter bootstrap code.
  // Regular hardware is supposed to only accept a STARTUP
  // when it is in the halted state due to an INIT.  So the second
  // should be ignored, but it is part of the official Intel algorithm.
  // Bochs complains about the second one.  Too bad for Bochs.
  for(i = 0; i < 2; i++){
    xapicw(ICRHI, hwid.num<<24);
    xapicw(ICRLO, STARTUP | (addr>>12));
    microdelay(200);
  }
}
