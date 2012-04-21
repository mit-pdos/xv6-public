#include "types.h"
#include "amd64.h"
#include "bits.hh"
#include "kernel.hh"
#include "traps.h"

#define ID      0x802   // ID
#define VER     0x803   // Version
#define TPR     0x808   // Task Priority
#define EOI     0x80b   // EOI
#define SVR     0x80f   // Spurious Interrupt Vector
  #define ENABLE     0x00000100   // Unit Enable
#define ESR     0x828   // Error Status
#define ICR     0x830   // Interrupt Command
  #define INIT       0x00000500   // INIT/RESET
  #define STARTUP    0x00000600   // Startup IPI
  #define BCAST      0x00080000   // Send to all APICs, including self.
  #define LEVEL      0x00008000   // Level triggered
//  #define DELIVS     0x00001000   // Delivery status
  #define ASSERT     0x00004000   // Assert interrupt (vs deassert)
#define TIMER   0x832   // Local Vector Table 0 (TIMER)
  #define X1         0x0000000B   // divide counts by 1
  #define PERIODIC   0x00020000   // Periodic
#define PCINT   0x834   // Performance Counter LVT
#define LINT0   0x835   // Local Vector Table 1 (LINT0)
#define LINT1   0x836   // Local Vector Table 2 (LINT1)
#define ERROR   0x837   // Local Vector Table 3 (ERROR)
  #define MASKED     0x00010000   // Interrupt masked
  #define MT_EXTINT  0x00000500
  #define MT_NMI     0x00000400   // NMI message type
  #define MT_FIX     0x00000000   // Fixed message type
#define TICR    0x838   // Timer Initial Count
#define TCCR    0x839   // Timer Current Count
#define TDCR    0x83e   // Timer Divide Configuration

#define IO_RTC  0x70

static u64 x2apichz;

static int
x2apicmaxlvt(void)
{
  unsigned int v;

  v = readmsr(VER);
  return (((v) >> 16) & 0xFFu);
}

void
x2apiceoi(void)
{
  writemsr(EOI, 0);
}

static int
x2apicwait(void)
{
  // Do nothing on x2apic
  return 0;
}

// Code closely follows native_cpu_up, do_boot_cpu,
// and wakeup_secondary_cpu_via_init in Linux v3.3
void
x2apicstartap(hwid_t id, u32 addr)
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

  // Be paranoid about clearing APIC errors
  writemsr(ESR, 0);
  readmsr(ESR);

  unsigned long accept_status;
  int maxlvt = x2apicmaxlvt();
 
  if (maxlvt > 3)
    writemsr(ESR, 0);
  readmsr(ESR);

  // "Universal startup algorithm."
  // Send INIT (level-triggered) interrupt to reset other CPU.

  // Asserting INIT
  writemsr(ICR, (((u64)id.num)<<32) | INIT | LEVEL | ASSERT);
  microdelay(10000);
  // Deasserting INIT
  writemsr(ICR, (((u64)id.num)<<32) | INIT | LEVEL);
  x2apicwait();

  // Send startup IPI (twice!) to enter bootstrap code.
  // Regular hardware is supposed to only accept a STARTUP
  // when it is in the halted state due to an INIT.  So the second
  // should be ignored, but it is part of the official Intel algorithm.
  // Bochs complains about the second one.  Too bad for Bochs.
  for(i = 0; i < 2; i++){
    if (maxlvt > 3)
      writemsr(ESR, 0);
    readmsr(ESR);

    // Kick the target chip
    writemsr(ICR, (((u64)id.num)<<32) | STARTUP | (addr>>12));
    // Linux gives 300 usecs to accept the IPI..
    microdelay(300);
    x2apicwait();
    // ..then it gives some more time
    microdelay(200);

    if (maxlvt > 3)
      writemsr(ESR, 0);

    accept_status = (readmsr(ESR) & 0xEF);
    if (accept_status)
      panic("x2apicstartap: accept status %lx", accept_status);
  }
}

void
x2apic_tlbflush(hwid_t id)
{
  panic("x2apic_tlbflush");
}

void
x2apic_sampconf(hwid_t id)
{
  panic("x2apic_sampconf");
}

void
x2apicpc(char mask)
{
  writemsr(PCINT, mask ? MASKED : MT_NMI);
}

hwid_t
x2apicid(void)
{
  u64 id = readmsr(ID);
  return HWID((u32)id);
}

void
initx2apic(void)
{
  u64 count;

  // Enable local APIC; set spurious interrupt vector.
  writemsr(SVR, ENABLE | (T_IRQ0 + IRQ_SPURIOUS));

  if (x2apichz == 0) {
    // Measure the TICR frequency
    writemsr(TDCR, X1);    
    writemsr(TICR, 0xffffffff); 
    u64 ccr0 = readmsr(TCCR);
    microdelay(10 * 1000);    // 1/100th of a second
    u64 ccr1 = readmsr(TCCR);
    x2apichz = 100 * (ccr0 - ccr1);
  }

  count = (QUANTUM*x2apichz) / 1000;
  if (count > 0xffffffff)
    panic("initx2apic: QUANTUM too large");

  // The timer repeatedly counts down at bus frequency
  // from xapic[TICR] and then issues an interrupt.  
  writemsr(TDCR, X1);
  writemsr(TIMER, PERIODIC | (T_IRQ0 + IRQ_TIMER));
  writemsr(TICR, count); 

  // Disable logical interrupt lines.
  writemsr(LINT0, MASKED);
  writemsr(LINT1, MASKED);

  // Enable performance counter overflow interrupts for sampler.cc
  if (((readmsr(VER)>>16) & 0xFF) >= 4)
    x2apicpc(0);

  // Map error interrupt to IRQ_ERROR.
  writemsr(ERROR, T_IRQ0 + IRQ_ERROR);

  // Clear error status register (requires back-to-back writes).
  writemsr(ESR, 0);
  writemsr(ESR, 0);

  // Ack any outstanding interrupts.
  writemsr(EOI, 0);

  // Send an Init Level De-Assert to synchronise arbitration ID's.
  writemsr(ICR, BCAST | INIT | LEVEL);

  // Enable interrupts on the APIC (but not on the processor).
  writemsr(TPR, 0);
}
