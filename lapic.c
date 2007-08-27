#include "types.h"
#include "mp.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "traps.h"
#include "mmu.h"
#include "proc.h"
#include "lapic.h"

// Local APIC registers, divided by 4 for use as uint[] indices.
#define ID      (0x0020/4)   // ID
#define VER     (0x0030/4)   // Version
#define TPR     (0x0080/4)   // Task Priority
#define APR     (0x0090/4)   // Arbitration Priority
#define PPR     (0x00A0/4)   // Processor Priority
#define EOI     (0x00B0/4)   // EOI
#define LDR     (0x00D0/4)   // Logical Destination
#define DFR     (0x00E0/4)   // Destination Format
#define SVR     (0x00F0/4)   // Spurious Interrupt Vector
#define ISR     (0x0100/4)   // Interrupt Status (8 registers)
#define TMR     (0x0180/4)   // Trigger Mode (8 registers)
#define IRR     (0x0200/4)   // Interrupt Request (8 registers)
#define ESR     (0x0280/4)   // Error Status
#define ICRLO   (0x0300/4)   // Interrupt Command
#define ICRHI   (0x0310/4)   // Interrupt Command [63:32]
#define TIMER   (0x0320/4)   // Local Vector Table 0 (TIMER)
#define PCINT   (0x0340/4)   // Performance Counter LVT
#define LINT0   (0x0350/4)   // Local Vector Table 1 (LINT0)
#define LINT1   (0x0360/4)   // Local Vector Table 2 (LINT1)
#define ERROR   (0x0370/4)   // Local Vector Table 3 (ERROR)
#define TICR    (0x0380/4)   // Timer Initial Count
#define TCCR    (0x0390/4)   // Timer Current Count
#define TDCR    (0x03E0/4)   // Timer Divide Configuration

// SVR  
#define ENABLE     0x00000100   // Unit Enable
#define FOCUS      0x00000200   // Focus Processor Checking Disable

// ICRLO
// [14] IPI Trigger Mode Level (RW)
#define DEASSERT   0x00000000   // Deassert level-sensitive interrupt
#define ASSERT     0x00004000   // Assert level-sensitive interrupt

// [17:16] Remote Read Status
#define INVALID    0x00000000   // Invalid
#define WAIT       0x00010000   // In-Progress
#define VALID      0x00020000   // Valid

// [19:18] Destination Shorthand
#define FIELD      0x00000000   // No shorthand
#define SELF       0x00040000   // Self is single destination
#define ALLINC     0x00080000   // All including self
#define ALLEXC     0x000C0000   // All Excluding self

// ESR
#define SENDCS     0x00000001   // Send CS Error
#define RCVCS      0x00000002   // Receive CS Error
#define SENDACCEPT 0x00000004   // Send Accept Error
#define RCVACCEPT  0x00000008   // Receive Accept Error
#define SENDVECTOR 0x00000020   // Send Illegal Vector
#define RCVVECTOR  0x00000040   // Receive Illegal Vector
#define REGISTER   0x00000080   // Illegal Register Address

// [17] Timer Mode (RW)
#define ONESHOT    0x00000000   // One-shot
#define PERIODIC   0x00020000   // Periodic

// [19:18] Timer Base (RW)
#define CLKIN      0x00000000   // use CLKIN as input
#define TMBASE     0x00040000   // use TMBASE
#define DIVIDER    0x00080000   // use output of the divider

#define X2         0x00000000   // divide by 2
#define X4         0x00000001   // divide by 4
#define X8         0x00000002   // divide by 8
#define X16        0x00000003   // divide by 16
#define X32        0x00000008   // divide by 32
#define X64        0x00000009   // divide by 64
#define X128       0x0000000A   // divide by 128
#define X1         0x0000000B   // divide by 1

//PAGEBREAK!
volatile uint *lapic;  // Initialized in mp.c

void
lapic_init(int c)
{
  uint r, lvt;

  if(!lapic) 
    return;

  lapic[DFR] = 0xFFFFFFFF;    // Set dst format register
  r = (lapic[ID]>>24) & 0xFF; // Read APIC ID
  lapic[LDR] = (1<<r) << 24;
  lapic[TPR] = 0xFF;          // No interrupts for now

  // Enable APIC
  lapic[SVR] = ENABLE | (IRQ_OFFSET+IRQ_SPURIOUS);

  // In virtual wire mode, set up the LINT0 and LINT1 as follows:
  lapic[LINT0] = APIC_IMASK | APIC_EXTINT;
  lapic[LINT1] = APIC_IMASK | APIC_NMI;

  lapic[EOI] = 0; // Ack any outstanding interrupts.

  lvt = (lapic[VER]>>16) & 0xFF;
  if(lvt >= 4)
    lapic[PCINT] = APIC_IMASK;
  lapic[ERROR] = IRQ_OFFSET+IRQ_ERROR;
  lapic[ESR] = 0;
  lapic[ESR];

  // Issue an INIT Level De-Assert to synchronise arbitration ID's.
  lapic[ICRHI] = 0;
  lapic[ICRLO] = ALLINC | APIC_LEVEL |
                       DEASSERT | APIC_INIT;
  while(lapic[ICRLO] & APIC_DELIVS)
    ;

  // Initialize the interrupt timer.
  // On real hardware would need to do more XXX.
  lapic[TDCR] = X1;
  lapic[TIMER] = CLKIN | PERIODIC | (IRQ_OFFSET + IRQ_TIMER);
  lapic[TCCR] = 10000000;
  lapic[TICR] = 10000000;

  // Enable interrupts on the APIC (but not on processor).
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

// Start additional processor running bootstrap code at addr.
void
lapic_startap(uchar apicid, uint addr)
{
  int i;
  volatile int j = 0;

  lapic[ICRHI] = apicid<<24;
  lapic[ICRLO] = FIELD | APIC_LEVEL | ASSERT | APIC_INIT;
  for(j=0; j<10000; j++);  // 200us
  lapic[ICRLO] = FIELD | APIC_LEVEL | DEASSERT | APIC_INIT;
  for(j=0; j<1000000; j++);  // 10ms

  for(i = 0; i < 2; i++){
    lapic[ICRHI] = apicid<<24;
    lapic[ICRLO] = FIELD | APIC_EDGE | APIC_STARTUP | (addr/4096);
    for(j=0; j<10000; j++);  // 200us
  }
}
