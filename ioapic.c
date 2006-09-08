#include "types.h"
#include "mp.h"
#include "defs.h"
#include "x86.h"
#include "traps.h"
#include "ioapic.h"

struct ioapic {
  uint ioregsel;  uint p01; uint p02; uint p03;
  uint iowin;     uint p11; uint p12; uint p13;
};


#define IOAPIC_REDTBL_LO(i)  (IOAPIC_REDTBL + (i) * 2)
#define IOAPIC_REDTBL_HI(i)  (IOAPIC_REDTBL_LO(i) + 1)

static uint
ioapic_read(struct ioapic *io, int reg)
{
  io->ioregsel = reg;
  return io->iowin;
}

static void
ioapic_write(struct ioapic *io, int reg, uint val)
{
  io->ioregsel = reg;
  io->iowin = val;
}

void
ioapic_init(void)
{
  struct ioapic *io;
  uint l, h;
  int nintr;
  uchar id;
  int i;

  if(!ismp)
    return;

  io = (struct ioapic*) IO_APIC_BASE;
  l = ioapic_read(io, IOAPIC_VER);
  nintr =  ((l & IOART_VER_MAXREDIR) >> MAXREDIRSHIFT) + 1;
  id = ioapic_read(io, IOAPIC_ID) >> APIC_ID_SHIFT;
  if(id != ioapic_id)
    cprintf("ioapic_init: id isn't equal to ioapic_id; not a MP\n");
  for(i = 0; i < nintr; i++) {
    // active-hi and edge-triggered for ISA interrupts
    // Assume that pin 0 on the first I/O APIC is an ExtINT pin.
    // Assume that pins 1-15 are ISA interrupts
    l = ioapic_read(io, IOAPIC_REDTBL_LO(i));
    l = l & ~IOART_INTMASK;  // allow INTs
    l |= IOART_INTMSET;
    l = l & ~IOART_INTPOL;   // active hi
    l = l & ~IOART_TRGRMOD;  // edgee triggered
    l = l & ~IOART_DELMOD;   // fixed
    l = l & ~IOART_DESTMOD;  // physical mode
    l = l | (IRQ_OFFSET + i); // vector
    ioapic_write(io, IOAPIC_REDTBL_LO(i), l);
    h = ioapic_read(io, IOAPIC_REDTBL_HI(i));
    h &= ~IOART_DEST;
    ioapic_write(io, IOAPIC_REDTBL_HI(i), h);
  }
}

void
ioapic_enable (int irq, int cpunum)
{
  uint l, h;
  struct ioapic *io;
  
  if(!ismp)
    return;

  io = (struct ioapic*) IO_APIC_BASE;
  l = ioapic_read(io, IOAPIC_REDTBL_LO(irq));
  l = l & ~IOART_INTMASK;  // allow INTs
  ioapic_write(io, IOAPIC_REDTBL_LO(irq), l);
  h = ioapic_read(io, IOAPIC_REDTBL_HI(irq));
  h &= ~IOART_DEST;
  h |= (cpunum << APIC_ID_SHIFT);
  ioapic_write(io, IOAPIC_REDTBL_HI(irq), h);
}
