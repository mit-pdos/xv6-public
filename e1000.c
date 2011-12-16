#include "types.h"
#include "amd64.h"
#include "kernel.h"
#include "pci.h"
#include "e1000reg.h"

static struct {
  u32 membase;
  u32 iobase;
  u16 pcidevid;
} e1000;

static u32
e1000_io_read(u32 reg)
{
  paddr pa = e1000.membase + reg;
  volatile u32 *ptr = p2v(pa);
  return *ptr;
}

static void
e1000_io_write(u32 reg, u32 val)
{
  paddr pa = e1000.membase + reg;
  volatile u32 *ptr = p2v(pa);
  *ptr = val;
}

static int
eeprom_eerd_read(u16 off)
{
  u32 reg;
  int x;

  // [E1000 5.3.1] Software EEPROM access 
  e1000_io_write(WMREG_EERD, (off<<EERD_ADDR_SHIFT) | EERD_START);
  for (x = 0; x < 100; x++) {
    reg = e1000_io_read(WMREG_EERD);
    if (reg & EERD_DONE)
      return (reg&EERD_DATA_MASK) >> EERD_DATA_SHIFT;
    microdelay(50000);
  }
  return -1;
}

static int
eeprom_read(u16 *buf, int off, int count)
{
  for (int i = 0; i < count; i++) {
    int r = eeprom_eerd_read(off+i);
    if (r < 0) {
      cprintf("eeprom_read: cannot read\n");
      return -1;
    }
    buf[i] = r;
  }
  return 0;
}

int
e1000attach(struct pci_func *pcif)
{
  int r;

  pci_func_enable(pcif);
  
  e1000.membase = pcif->reg_base[0];
  e1000.iobase = pcif->reg_base[2];
  e1000.pcidevid = pcif->dev_id;

  // Get the MAC address
  u16 myaddr[3];
  r = eeprom_read(&myaddr[0], EEPROM_OFF_MACADDR, 3);
  if (r < 0)
    return 0;
  u8 *addr = (u8*) &myaddr[0];
  cprintf("%x:%x:%x:%x:%x:%x\n",
          addr[0], addr[1], addr[2],
          addr[3], addr[4], addr[5]);

  return 0;
}
