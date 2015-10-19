// The I/O APIC manages hardware interrupts for an SMP system.
// http://www.intel.com/design/chipsets/datashts/29056601.pdf
// See also picirq.c.

#include "types.h"
#include "defs.h"
#include "traps.h"

#define IOAPIC  0xFEC00000   // Default physical address of IO APIC

#define REG_ID     0x00  // Register index: ID
#define REG_VER    0x01  // Register index: version
#define REG_ARB    0x02  // Register index: arbitration ID
#define REG_TABLE  0x10  // Redirection table base

// The redirection table starts at REG_TABLE and uses
// two registers to configure each interrupt.
// The first (low) register in a pair contains configuration bits.
// The second (high) register contains a bitmask telling which
// CPUs can serve that interrupt.
#define INT_DISABLED   (1 << 16)  // Interrupt disabled
#define INT_LEVEL      (1 << 15)  // Level-triggered (vs edge-)
#define INT_ACTIVELOW  (1 << 13)  // Active low (vs high)
#define INT_LOGICAL    (1 << 11)  // Destination is CPU id (vs APIC ID)

volatile struct ioapic *ioapic;  // Initialized in mp.c
static int maxintr;  // Maximum number of interrupts

// IO APIC MMIO structure: write reg, then read or write data.
struct ioapic {
  uint reg;     // offset  00
  uint pad[3];  // offsets 04 08 0C
  uint data;    // offset  10
};

static uint
ioapicread(int reg)
{
  ioapic->reg = (reg & 0xFF);  // bits 0..7, see datasheet
  return ioapic->data;
}

static void
ioapicwrite(int reg, uint data)
{
  ioapic->reg = (reg & 0xFF);  // bits 0..7, see datasheet
  ioapic->data = data;
}

void
ioapicinit(void)
{
  int i, id;

  if(!ismp)
    return;

  if(ioapic == 0) {
    ioapic = (volatile struct ioapic*)IOAPIC;
    cprintf("ioapicinit: falling back to default ioapic address\n");
  }
  maxintr = (ioapicread(REG_VER) >> 16) & 0xFF; // bits 16..23, see datasheet
  id = (ioapicread(REG_ID) >> 24) & 0x0F;  // bits 24..27, see datasheet
  if(id != ioapicid)
    cprintf("ioapicinit: id isn't equal to ioapicid; not a MP\n");

  // Mark all interrupts edge-triggered, active high, disabled,
  // and not routed to any CPUs.
  for(i = 0; i <= maxintr; i++){
    ioapicwrite(REG_TABLE+2*i, INT_DISABLED | (T_IRQ0 + i));
    ioapicwrite(REG_TABLE+2*i+1, 0);
  }
}

void
ioapicenable(int irq, int cpunum)
{
  if(!ismp)
    return;

  if(irq > maxintr)
    cprintf("ioapicenable: no irq %d, maximum is %d\n", irq, maxintr);

  cpunum = cpunum & 0x0F; // used as APIC id below, 4 bits, see datasheet

  // Mark interrupt edge-triggered, active high,
  // enabled, and routed to the given cpunum,
  // which happens to be that cpu's APIC ID.
  ioapicwrite(REG_TABLE+2*irq, T_IRQ0 + irq);
  ioapicwrite(REG_TABLE+2*irq+1, cpunum << 24); // bits 56..59, see datasheet
}
