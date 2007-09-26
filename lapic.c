// The local APIC manages internal (non-I/O) interrupts.
// See Chapter 8 & Appendix C of Intel processor manual volume 3.

#include "types.h"
#include "traps.h"

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
  #define LEVEL      0x00008000   // Level triggered
  #define BCAST      0x00080000   // Send to all APICs, including self.
#define ICRHI   (0x0310/4)   // Interrupt Command [63:32]
#define TIMER   (0x0320/4)   // Local Vector Table 0 (TIMER)
  #define X1         0x0000000B   // divide counts by 1
  #define PERIODIC   0x00020000   // Periodic
#define PCINT   (0x0340/4)   // Performance Counter LVT
#define LINT0   (0x0350/4)   // Local Vector Table 1 (LINT0)
#define LINT1   (0x0360/4)   // Local Vector Table 2 (LINT1)
#define ERROR   (0x0370/4)   // Local Vector Table 3 (ERROR)
  #define MASKED     0x00010000   // Interrupt masked
#define TICR    (0x0380/4)   // Timer Initial Count
#define TCCR    (0x0390/4)   // Timer Current Count
#define TDCR    (0x03E0/4)   // Timer Divide Configuration

volatile uint *lapic;  // Initialized in mp.c

//PAGEBREAK!
void
lapic_init(int c)
{
  if(!lapic) 
    return;

  // Enable local APIC; set spurious interrupt vector.
  lapic[SVR] = ENABLE | (IRQ_OFFSET+IRQ_SPURIOUS);

  // The timer repeatedly counts down at bus frequency
  // from lapic[TICR] and then issues an interrupt.  
  // If xv6 cared more about precise timekeeping,
  // TICR would be calibrated using an external time source.
  lapic[TDCR] = X1;
  lapic[TIMER] = PERIODIC | (IRQ_OFFSET + IRQ_TIMER);
  lapic[TICR] = 10000000; 

  // Disable logical interrupt lines.
  lapic[LINT0] = MASKED;
  lapic[LINT1] = MASKED;

  // Disable performance counter overflow interrupts
  // on machines that provide that interrupt entry.
  if(((lapic[VER]>>16) & 0xFF) >= 4)
    lapic[PCINT] = MASKED;

  // Map error interrupt to IRQ_ERROR.
  lapic[ERROR] = IRQ_OFFSET+IRQ_ERROR;

  // Clear error status register (requires back-to-back writes).
  lapic[ESR] = 0;
  lapic[ESR] = 0;

  // Ack any outstanding interrupts.
  lapic[EOI] = 0;

  // Send an Init Level De-Assert to synchronise arbitration ID's.
  lapic[ICRHI] = 0;
  lapic[ICRLO] = BCAST | INIT | LEVEL;
  while(lapic[ICRLO] & DELIVS)
    ;

  // Enable interrupts on the APIC (but not on the processor).
  lapic[TPR] = 0;
}

int
cpu(void)
{
  if(lapic)
    return lapic[ID]>>24;
  return 0;
}

// Acknowledge interrupt.
void
lapic_eoi(void)
{
  if(lapic)
    lapic[EOI] = 0;
}

// Spin for a given number of microseconds.
// On real hardware would want to tune this dynamically.
static void
microdelay(int us)
{
  volatile int j = 0;
  
  while(us-- > 0)
    for(j=0; j<10000; j++);
}

// Start additional processor running bootstrap code at addr.
// See Appendix B of MultiProcessor Specification.
void
lapic_startap(uchar apicid, uint addr)
{
  int i;
  volatile int j = 0;

  // Send INIT interrupt to reset other CPU.
  lapic[ICRHI] = apicid<<24;
  lapic[ICRLO] = INIT | LEVEL;
  microdelay(10);
  
  // Send startup IPI (twice!) to enter bootstrap code.
  // Regular hardware wants it twice, but Bochs complains.
  // Too bad for Bochs.
  for(i = 0; i < 2; i++){
    lapic[ICRHI] = apicid<<24;
    lapic[ICRLO] = STARTUP | (addr>>12);
    for(j=0; j<10000; j++);  // 200us
  }
}
