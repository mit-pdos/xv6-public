#include "types.h"
#include "mp.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "traps.h"
#include "mmu.h"
#include "proc.h"

enum {  // Local APIC registers
  LAPIC_ID  = 0x0020,   // ID
  LAPIC_VER = 0x0030,   // Version
  LAPIC_TPR = 0x0080,   // Task Priority
  LAPIC_APR = 0x0090,   // Arbitration Priority
  LAPIC_PPR = 0x00A0,   // Processor Priority
  LAPIC_EOI = 0x00B0,   // EOI
  LAPIC_LDR = 0x00D0,   // Logical Destination
  LAPIC_DFR = 0x00E0,   // Destination Format
  LAPIC_SVR = 0x00F0,   // Spurious Interrupt Vector
  LAPIC_ISR = 0x0100,   // Interrupt Status (8 registers)
  LAPIC_TMR = 0x0180,   // Trigger Mode (8 registers)
  LAPIC_IRR = 0x0200,   // Interrupt Request (8 registers)
  LAPIC_ESR = 0x0280,   // Error Status
  LAPIC_ICRLO = 0x0300, // Interrupt Command
  LAPIC_ICRHI = 0x0310, // Interrupt Command [63:32]
  LAPIC_TIMER = 0x0320, // Local Vector Table 0 (TIMER)
  LAPIC_PCINT = 0x0340, // Performance Counter LVT
  LAPIC_LINT0 = 0x0350, // Local Vector Table 1 (LINT0)
  LAPIC_LINT1 = 0x0360, // Local Vector Table 2 (LINT1)
  LAPIC_ERROR = 0x0370, // Local Vector Table 3 (ERROR)
  LAPIC_TICR = 0x0380,  // Timer Initial Count
  LAPIC_TCCR = 0x0390,  // Timer Current Count
  LAPIC_TDCR = 0x03E0,  // Timer Divide Configuration
};

enum {  // LAPIC_SVR
  LAPIC_ENABLE  = 0x00000100,   // Unit Enable
  LAPIC_FOCUS   = 0x00000200,   // Focus Processor Checking Disable
};

enum {  // LAPIC_ICRLO
  // [14] IPI Trigger Mode Level (RW)
  LAPIC_DEASSERT = 0x00000000,  // Deassert level-sensitive interrupt
  LAPIC_ASSERT  = 0x00004000,   // Assert level-sensitive interrupt

  // [17:16] Remote Read Status
  LAPIC_INVALID = 0x00000000,   // Invalid
  LAPIC_WAIT    = 0x00010000,   // In-Progress
  LAPIC_VALID   = 0x00020000,   // Valid

  // [19:18] Destination Shorthand
  LAPIC_FIELD   = 0x00000000,   // No shorthand
  LAPIC_SELF    = 0x00040000,   // Self is single destination
  LAPIC_ALLINC  = 0x00080000,   // All including self
  LAPIC_ALLEXC  = 0x000C0000,   // All Excluding self
};

enum {  // LAPIC_ESR
  LAPIC_SENDCS  = 0x00000001,     // Send CS Error
  LAPIC_RCVCS   = 0x00000002,     // Receive CS Error
  LAPIC_SENDACCEPT = 0x00000004,  // Send Accept Error
  LAPIC_RCVACCEPT = 0x00000008,   // Receive Accept Error
  LAPIC_SENDVECTOR = 0x00000020,  // Send Illegal Vector
  LAPIC_RCVVECTOR = 0x00000040,   // Receive Illegal Vector
  LAPIC_REGISTER = 0x00000080,    // Illegal Register Address
};

enum {  // LAPIC_TIMER
  // [17] Timer Mode (RW)
  LAPIC_ONESHOT = 0x00000000,   // One-shot
  LAPIC_PERIODIC = 0x00020000,  // Periodic

  // [19:18] Timer Base (RW)
  LAPIC_CLKIN   = 0x00000000,   // use CLKIN as input
  LAPIC_TMBASE  = 0x00040000,   // use TMBASE
  LAPIC_DIVIDER = 0x00080000,   // use output of the divider
};

enum {  // LAPIC_TDCR
  LAPIC_X2 = 0x00000000,        // divide by 2
  LAPIC_X4 = 0x00000001,        // divide by 4
  LAPIC_X8 = 0x00000002,        // divide by 8
  LAPIC_X16 = 0x00000003,       // divide by 16
  LAPIC_X32 = 0x00000008,       // divide by 32
  LAPIC_X64 = 0x00000009,       // divide by 64
  LAPIC_X128 = 0x0000000A,      // divide by 128
  LAPIC_X1 = 0x0000000B,        // divide by 1
};

uint *lapicaddr;

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
lapic_timerinit(void)
{
  if(!lapicaddr) 
    return;

  lapic_write(LAPIC_TDCR, LAPIC_X1);
  lapic_write(LAPIC_TIMER, LAPIC_CLKIN | LAPIC_PERIODIC |
	      (IRQ_OFFSET + IRQ_TIMER));
  lapic_write(LAPIC_TCCR, 10000000);
  lapic_write(LAPIC_TICR, 10000000);
}

void
lapic_timerintr(void)
{
  if(lapicaddr) 
    lapic_write(LAPIC_EOI, 0);
}

void
lapic_init(int c)
{
  uint r, lvt;

  if(!lapicaddr) 
    return;

  lapic_write(LAPIC_DFR, 0xFFFFFFFF);    // Set dst format register
  r = (lapic_read(LAPIC_ID)>>24) & 0xFF; // Read APIC ID
  lapic_write(LAPIC_LDR, (1<<r)<<24);    // Set logical dst register to r
  lapic_write(LAPIC_TPR, 0xFF);          // No interrupts for now

  // Enable APIC
  lapic_write(LAPIC_SVR, LAPIC_ENABLE|(IRQ_OFFSET+IRQ_SPURIOUS));

  // In virtual wire mode, set up the LINT0 and LINT1 as follows:
  lapic_write(LAPIC_LINT0, APIC_IMASK | APIC_EXTINT);
  lapic_write(LAPIC_LINT1, APIC_IMASK | APIC_NMI);

  lapic_write(LAPIC_EOI, 0); // Ack any outstanding interrupts.

  lvt = (lapic_read(LAPIC_VER)>>16) & 0xFF;
  if(lvt >= 4)
    lapic_write(LAPIC_PCINT, APIC_IMASK);
  lapic_write(LAPIC_ERROR, IRQ_OFFSET+IRQ_ERROR);
  lapic_write(LAPIC_ESR, 0);
  lapic_read(LAPIC_ESR);

  // Issue an INIT Level De-Assert to synchronise arbitration ID's.
  lapic_write(LAPIC_ICRHI, 0);
  lapic_write(LAPIC_ICRLO, LAPIC_ALLINC|APIC_LEVEL|
                           LAPIC_DEASSERT|APIC_INIT);
  while(lapic_read(LAPIC_ICRLO) & APIC_DELIVS)
    ;
}

void
lapic_enableintr(void)
{
  if(lapicaddr)
    lapic_write(LAPIC_TPR, 0);
}

void
lapic_disableintr(void)
{
  if(lapicaddr)
    lapic_write(LAPIC_TPR, 0xFF);
}

void
lapic_eoi(void)
{
  if(lapicaddr)
    lapic_write(LAPIC_EOI, 0);
}

int
cpu(void)
{
  int x;
  if(lapicaddr) 
    x = (lapic_read(LAPIC_ID)>>24) & 0xFF;
  else 
    x = 0;
  return x;
}

void
lapic_startap(uchar apicid, int v)
{
  int crhi, i;
  volatile int j = 0;

  crhi = apicid<<24;
  lapic_write(LAPIC_ICRHI, crhi);
  lapic_write(LAPIC_ICRLO, LAPIC_FIELD|APIC_LEVEL|
                           LAPIC_ASSERT|APIC_INIT);

  while(j++ < 10000) {;}
  lapic_write(LAPIC_ICRLO, LAPIC_FIELD|APIC_LEVEL|
                           LAPIC_DEASSERT|APIC_INIT);

  while(j++ < 1000000) {;}

  // in p9 code, this was i < 2, which is what the spec says on page B-3
  for(i = 0; i < 1; i++){
    lapic_write(LAPIC_ICRHI, crhi);
    lapic_write(LAPIC_ICRLO, LAPIC_FIELD|APIC_EDGE|APIC_STARTUP|(v/4096));
    while(j++ < 100000) {;}
  }
}
