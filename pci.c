#include "types.h"
#include "amd64.h"
#include "kernel.h"
#include "pci.h"
#include "pcireg.h"

extern int e1000attach(struct pci_func *pcif);

// Flag to do "lspci" at bootup
static int pci_show_devs = 1;
static int pci_show_addrs = 1;

// PCI "configuration mechanism one"
static u32 pci_conf1_addr_ioport = 0x0cf8;
static u32 pci_conf1_data_ioport = 0x0cfc;

// PCI driver table
struct pci_driver {
  u32 key1, key2;
  int (*attachfn) (struct pci_func *pcif);
};

// Forward declarations
static int pci_bridge_attach(struct pci_func *pcif);

// pci_attach_class matches the class and subclass of a PCI device
struct pci_driver pci_attach_class[] = {
  { PCI_CLASS_BRIDGE, PCI_SUBCLASS_BRIDGE_PCI, &pci_bridge_attach },
  { 0, 0, 0 },
};

// pci_attach_vendor matches the vendor ID and device ID of a PCI device
struct pci_driver pci_attach_vendor[] = {
  // [E1000 5.2] 
  // QEMU emulates an 82540EM, specifically.
  { 0x8086, 0x100e, &e1000attach },
  // Both of ud0's e1000e
  { 0x8086, 0x108c, &e1000attach },
  { 0x8086, 0x109A, &e1000attach },
  { 0, 0, 0 },
};

static const char *pci_class[] =
{
  [0x0] = "Unknown",
  [0x1] = "Storage controller",
  [0x2] = "Network controller",
  [0x3] = "Display controller",
  [0x4] = "Multimedia device",
  [0x5] = "Memory controller",
  [0x6] = "Bridge device",
};

static void
pci_print_func(struct pci_func *f)
{
  const char *class = pci_class[0];
  if (PCI_CLASS(f->dev_class) < sizeof(pci_class) / sizeof(pci_class[0]))
    class = pci_class[PCI_CLASS(f->dev_class)];

  cprintf("PCI: %x:%x.%d: %x:%x: class: %x.%x (%s) irq: %d\n",
          f->bus->busno, f->dev, f->func,
          PCI_VENDOR(f->dev_id), PCI_PRODUCT(f->dev_id),
          PCI_CLASS(f->dev_class), PCI_SUBCLASS(f->dev_class), class,
          f->irq_line);
}

static void
pci_conf1_set_addr(u32 bus,
		   u32 dev,
		   u32 func,
		   u32 offset)
{
  if (!(bus < 256 &&
        dev < 32 &&
        func < 8 &&
        offset < 256 &&
        (offset&0x3) == 0))
    panic("pci_conf1_set_addr");
  
  u32 v = (1 << 31) |		// config-space
    (bus << 16) | (dev << 11) | (func << 8) | (offset);
  outl(pci_conf1_addr_ioport, v);
}

static u32
pci_conf_read(struct pci_func *f, u32 off)
{
  pci_conf1_set_addr(f->bus->busno, f->dev, f->func, off);
  return inl(pci_conf1_data_ioport);
}

static void
pci_conf_write(struct pci_func *f, u32 off, u32 v)
{
  pci_conf1_set_addr(f->bus->busno, f->dev, f->func, off);
  outl(pci_conf1_data_ioport, v);
}

static int __attribute__((warn_unused_result))
pci_attach_match(u32 key1, u32 key2,
		 struct pci_driver *list, struct pci_func *pcif)
{
  u32 i;
  
  for (i = 0; list[i].attachfn; i++) {
    if (list[i].key1 == key1 && list[i].key2 == key2) {
      int r = list[i].attachfn(pcif);
      if (r > 0)
        return r;
      if (r < 0)
        cprintf("pci_attach_match: attaching "
                "%x.%x (%p): e\n",
                key1, key2, list[i].attachfn, r);
    }
  }
  return 0;
}

static int
pci_attach(struct pci_func *f)
{
  return
    pci_attach_match(PCI_CLASS(f->dev_class),
                     PCI_SUBCLASS(f->dev_class),
                     &pci_attach_class[0], f) ||
    pci_attach_match(PCI_VENDOR(f->dev_id),
                     PCI_PRODUCT(f->dev_id),
                     &pci_attach_vendor[0], f);
}

static int
pci_scan_bus(struct pci_bus *bus)
{
  int totaldev = 0;
  struct pci_func df;
  memset(&df, 0, sizeof(df));
  df.bus = bus;

  for (df.dev = 0; df.dev < 32; df.dev++) {
    u32 bhlc = pci_conf_read(&df, PCI_BHLC_REG);
    if (PCI_HDRTYPE_TYPE(bhlc) > 1)	    // Unsupported or no device
      continue;
    
    totaldev++;

    struct pci_func         f  = df;
    for (f.func = 0; f.func < (PCI_HDRTYPE_MULTIFN(bhlc) ? 8 : 1);
         f.func++) {
      struct pci_func af = f;
      
      af.dev_id                  = pci_conf_read(&f, PCI_ID_REG);
      if (PCI_VENDOR(af.dev_id) == 0xffff)
				continue;
      
      u32 intr = pci_conf_read(&af, PCI_INTERRUPT_REG);
      af.irq_line = PCI_INTERRUPT_LINE(intr);
      
      af.dev_class = pci_conf_read(&af, PCI_CLASS_REG);
      if (pci_show_devs)
        pci_print_func(&af);
      pci_attach(&af);
    }
  }
  return totaldev;
}

static int
pci_bridge_attach(struct pci_func *pcif)
{
  u32 ioreg  = pci_conf_read(pcif, PCI_BRIDGE_STATIO_REG);
  u32 busreg = pci_conf_read(pcif, PCI_BRIDGE_BUS_REG);

  if (PCI_BRIDGE_IO_32BITS(ioreg)) {
    cprintf("PCI: %x:%x.%d: 32-bit bridge IO not supported.\n",
            pcif->bus->busno, pcif->dev, pcif->func);
    return 0;
  }

  struct pci_bus nbus;
  memset(&nbus, 0, sizeof(nbus));
  nbus.parent_bridge = pcif;
  nbus.busno = (busreg >> PCI_BRIDGE_BUS_SECONDARY_SHIFT) & 0xff;

  if (pci_show_devs)
    cprintf("PCI: %x:%x.%d: bridge to PCI bus %d--%d\n",
            pcif->bus->busno, pcif->dev, pcif->func,
            nbus.busno,
            (busreg >> PCI_BRIDGE_BUS_SUBORDINATE_SHIFT) & 0xff);
  
  pci_scan_bus(&nbus);
  return 1;
}

void
pci_func_enable(struct pci_func *f)
{
  pci_conf_write(f, PCI_COMMAND_STATUS_REG,
                 PCI_COMMAND_IO_ENABLE |
                 PCI_COMMAND_MEM_ENABLE |
                 PCI_COMMAND_MASTER_ENABLE);
  
  u32 bar_width;
  u32 bar;
  for (bar = PCI_MAPREG_START; bar < PCI_MAPREG_END;
       bar += bar_width)
  {
    u32 oldv = pci_conf_read(f, bar);
    
    bar_width = 4;
    pci_conf_write(f, bar, 0xffffffff);
    u32 rv = pci_conf_read(f, bar);
    
    if (rv == 0)
      continue;
    
    int regnum = PCI_MAPREG_NUM(bar);
    u32 base, size;
    if (PCI_MAPREG_TYPE(rv) == PCI_MAPREG_TYPE_MEM) {
      if (PCI_MAPREG_MEM_TYPE(rv) == PCI_MAPREG_MEM_TYPE_64BIT)
        bar_width = 8;
      
      size = PCI_MAPREG_MEM_SIZE(rv);
      base = PCI_MAPREG_MEM_ADDR(oldv);
      if (pci_show_addrs)
        cprintf("  mem region %d: %d bytes at 0x%x\n",
                regnum, size, base);
    } else {
      size = PCI_MAPREG_IO_SIZE(rv);
      base = PCI_MAPREG_IO_ADDR(oldv);
      if (pci_show_addrs)
        cprintf("  io region %d: %d bytes at 0x%x\n",
                regnum, size, base);
    }
    
    pci_conf_write(f, bar, oldv);
    f->reg_base[regnum] = base;
    f->reg_size[regnum] = size;
    
    if (size && !base)
      cprintf("PCI device %02x:%02x.%d (%04x:%04x) "
              "may be misconfigured: "
              "region %d: base 0x%x, size %d\n",
              f->bus->busno, f->dev, f->func,
              PCI_VENDOR(f->dev_id), PCI_PRODUCT(f->dev_id),
              regnum, base, size);
  }
  
  cprintf("PCI function %x:%x.%d (%x:%x) enabled\n",
          f->bus->busno, f->dev, f->func,
          PCI_VENDOR(f->dev_id), PCI_PRODUCT(f->dev_id));
}

void
initpci(void)
{
  static struct pci_bus root_bus;
  memset(&root_bus, 0, sizeof(root_bus));

  pci_scan_bus(&root_bus);
}
