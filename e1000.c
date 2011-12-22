#include "types.h"
#include "amd64.h"
#include "kernel.h"
#include "pci.h"
#include "e1000reg.h"

#define TX_RING_SIZE 64
#define RX_RING_SIZE 64

int e1000irq;

static struct {
  u32 membase;
  u32 iobase;
  u16 pcidevid;
  
  u32 txclean;
  u32 txinuse;

  u32 rxclean;
  u32 rxuse;

  struct wiseman_txdesc txd[TX_RING_SIZE] __attribute__((aligned (16)));
  struct wiseman_rxdesc rxd[RX_RING_SIZE] __attribute__((aligned (16)));
} e1000;

static inline u32
erd(u32 reg)
{
  paddr pa = e1000.membase + reg;
  volatile u32 *ptr = p2v(pa);
  return *ptr;
}

static inline void
ewr(u32 reg, u32 val)
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
  ewr(WMREG_EERD, (off<<EERD_ADDR_SHIFT) | EERD_START);
  for (x = 0; x < 5; x++) {
    reg = erd(WMREG_EERD);
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
e1000tx(void *buf, u32 len)
{
  struct wiseman_txdesc *desc;
  u32 tail;

  // WMREG_TDT should only equal WMREG_TDH when we have
  // nothing to transmit.  Therefore, we can accomodate
  // TX_RING_SIZE-1 buffers.
  if (e1000.txinuse == TX_RING_SIZE-1) {
    cprintf("TX ring overflow\n");
    return -1;
  }

  tail = erd(WMREG_TDT);
  desc = &e1000.txd[tail];
  if (!(desc->wtx_fields.wtxu_status & WTX_ST_DD))
    panic("oops");

  desc->wtx_addr = v2p(buf);
  desc->wtx_cmdlen = len | WTX_CMD_RS | WTX_CMD_EOP | WTX_CMD_IFCS;
  memset(&desc->wtx_fields, 0, sizeof(&desc->wtx_fields));
  ewr(WMREG_TDT, (tail+1) % TX_RING_SIZE);
  e1000.txinuse++;

  return 0;
}

static void
cleantx(void)
{
  struct wiseman_txdesc *desc;
  void *va;

  while (e1000.txinuse) {
    desc = &e1000.txd[e1000.txclean];
    if (!(desc->wtx_fields.wtxu_status & WTX_ST_DD))
      break;

    va = p2v(desc->wtx_addr);
    netfree(va);
    desc->wtx_fields.wtxu_status = 0;

    e1000.txclean = (e1000.txclean+1) % TX_RING_SIZE;
    desc = &e1000.txd[e1000.txclean];
    e1000.txinuse--;
  }
}

static void
allocrx(void)
{
  struct wiseman_rxdesc *desc;
  void *buf;
  u32 i;

  i = erd(WMREG_RDT);
  desc = &e1000.rxd[i];  
  if (desc->wrx_status & WRX_ST_DD)
    panic("allocrx");
  buf = netalloc();
  if (buf == NULL)
    panic("Oops");
  desc->wrx_addr = v2p(buf);
  
  ewr(WMREG_RDT, (i+1) % RX_RING_SIZE);
}

static void
cleanrx(void)
{
  struct wiseman_rxdesc *desc;
  void *va;
  u16 len;
  u32 i;

  i = e1000.rxclean;
  desc = &e1000.rxd[i];

  while (desc->wrx_status & WRX_ST_DD) {
    va = p2v(desc->wrx_addr);
    len = desc->wrx_len;

    desc->wrx_status = 0;
    allocrx();
    netrx(va, len);
    
    i = (i+1) % RX_RING_SIZE;
    desc = &e1000.rxd[i];
  }
  e1000.rxclean = i;
}

void
e1000intr(void)
{
  u32 icr = erd(WMREG_ICR);

  while (icr & (ICR_TXDW|ICR_RXO|ICR_RXT0)) {
    if (icr & ICR_TXDW)
      cleantx();
	
    if (icr & ICR_RXT0)
      cleanrx();

    if (icr & ICR_RXO)
      panic("ICR_RXO");

    icr = erd(WMREG_ICR);
  }
}

static
void e1000reset(void)
{
  u32 ctrl;
  paddr tpa;
  paddr rpa;

  ctrl = erd(WMREG_CTRL);  
  // [E1000 13.4.1] Assert PHY_RESET then delay as much as 10 msecs
  // before clearing PHY_RESET.
  ewr(WMREG_CTRL, ctrl|CTRL_PHY_RESET);
  microdelay(10000);
  ewr(WMREG_CTRL, ctrl);

  // [E1000 13.4.1] Delay 1 usec after reset
  ewr(WMREG_CTRL, ctrl|CTRL_RST);
  microdelay(1);

  // [E1000 13.4.41] Transmit Interrupt Delay Value of 1 usec.
  // A value of 0 is not allowed.  Enabled on a per-TX decriptor basis.
  ewr(WMREG_TIDV, 1);
  // [E1000 13.4.44] Delay TX interrupts a max of 1 usec.
  ewr(WMREG_TADV, 1);
  for (int i = 0; i < TX_RING_SIZE; i++)
    e1000.txd[i].wtx_fields.wtxu_status = WTX_ST_DD;
  // [E1000 14.5] Transmit Initialization
  tpa = v2p(e1000.txd);
  ewr(WMREG_TDBAH, tpa >> 32);
  ewr(WMREG_TDBAL, tpa & 0xffffffff);
  ewr(WMREG_TDLEN, sizeof(e1000.txd));
  ewr(WMREG_TDH, 0);
  ewr(WMREG_TDT, 0);
  ewr(WMREG_TCTL, TCTL_EN|TCTL_PSP|TCTL_CT(0x10)|TCTL_COLD(0x40));
  ewr(WMREG_TIPG, TIPG_IPGT(10)|TIPG_IPGR1(8)|TIPG_IPGR2(6));

  for (int i = 0; i < RX_RING_SIZE>>1; i++) {
    void *buf = netalloc();
    e1000.rxd[i].wrx_addr = v2p(buf);
  }
  rpa = v2p(e1000.rxd);
  ewr(WMREG_RDBAH, rpa >> 32);
  ewr(WMREG_RDBAL, rpa & 0xffffffff);
  ewr(WMREG_RDLEN, sizeof(e1000.rxd));
  ewr(WMREG_RDH, 0);
  ewr(WMREG_RDT, RX_RING_SIZE>>1);
  ewr(WMREG_RDTR, 0);
  ewr(WMREG_RADV, 0);
  ewr(WMREG_RCTL,
      RCTL_EN | RCTL_RDMTS_1_2 | RCTL_DPF | RCTL_BAM | RCTL_2k);
  ewr(WMREG_RDT, RX_RING_SIZE-1);

  // [E1000 13.4.20]
  ewr(WMREG_IMC, ~0);
  ewr(WMREG_IMS, ICR_TXDW | ICR_RXO | ICR_RXT0);
}

//#define E1000_WRITE_FLUSH() erd(WMREG_STATUS)

int
e1000attach(struct pci_func *pcif)
{
  int r;

  pci_func_enable(pcif);
  
  e1000.membase = pcif->reg_base[0];
  e1000.iobase = pcif->reg_base[2];
  e1000.pcidevid = pcif->dev_id;
  e1000irq = pcif->irq_line;

  picenable(e1000irq);
  ioapicenable(e1000irq, 0);

  e1000reset();

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
