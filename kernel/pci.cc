#include "types.h"
#include "amd64.h"
#include "kernel.hh"
#include "pci.hh"
#include "pcireg.hh"
#include "traps.h"

extern int e1000attach(struct pci_func *pcif);
extern int e1000eattach(struct pci_func *pcif);

// Flag to do "lspci" at bootup
static int pci_show_devs = 0;
static int pci_show_addrs = 0;

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
  // QEMU emulates an e1000 82540EM
  { 0x8086, 0x100e, &e1000attach },
  // josmp's dual ported e1000 82546GB copper
  { 0x8086, 0x1079, &e1000attach },
  // tom's e1000 82541GI copper
  { 0x8086, 0x1076, &e1000attach },
  // Both of ud0's e1000e (82573E, 82573L)
  { 0x8086, 0x108c, &e1000eattach },
  { 0x8086, 0x109A, &e1000eattach },
  { 0, 0, 0 },
};

static const char *pci_class[] =
{
  "Unknown",
  "Storage controller",
  "Network controller",
  "Display controller",
  "Multimedia device",
  "Memory controller",
  "Bridge device",
};

static void
pci_print_func(struct pci_func *f)
{
  const char *classname = pci_class[0];
  if (PCI_CLASS(f->dev_class) < sizeof(pci_class) / sizeof(pci_class[0]))
    classname = pci_class[PCI_CLASS(f->dev_class)];

  cprintf("PCI: %x:%x.%d: %x:%x: class: %x.%x (%s) irq: %d\n",
          f->bus->busno, f->dev, f->func,
          PCI_VENDOR(f->dev_id), PCI_PRODUCT(f->dev_id),
          PCI_CLASS(f->dev_class), PCI_SUBCLASS(f->dev_class), classname,
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
                "%x.%x (%p): %d\n",
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

static void
pci_scan_caplist(struct pci_func* f)
{
  u32 cap_ptr = PCI_CAPLIST_PTR(pci_conf_read(f, PCI_CAPLISTPTR_REG));
  for (int i = 0; i < 10 && cap_ptr != 0; i++) {
    u32 cap_entry = pci_conf_read(f, cap_ptr);
    switch (PCI_CAPLIST_CAP(cap_entry)) {
    case PCI_CAP_MSI:
      f->msi_capreg = cap_ptr;
      break;
    default:
      break;
    }
    cap_ptr = PCI_CAPLIST_NEXT(cap_entry);
  }
}

void
pci_msi_enable(struct pci_func *f, u8 irqnum)
{
  // PCI System Architecture, Fourth Edition

  assert(f->msi_capreg != 0);
  u32 cap_entry = pci_conf_read(f, f->msi_capreg);  

  if (!(cap_entry & PCI_MSI_MCR_64BIT))
    panic("pci_msi_enable only handles 64-bit address capable devices");
  if (PCI_MSI_MCR_MMC(cap_entry) != 0)
    panic("pci_msi_enable only handles 1 requested message");

  // [PCI SA pg 253]
  // Step 4. Assign a dword-aligned memory address to the device's
  // Message Address Register.
  // (The Message Address Register format is mandated by the x86
  // architecture.  See 9.11.1 in the Vol. 3 of the Intel architecture
  // manual.)
  pci_conf_write(f, f->msi_capreg + 4*1,
                 (0x0fee << 20) |   // magic constant for northbridge
                 (0 << 12) |        // destination ID
                 (1 << 3) |         // redirection hint
                 (0 << 2));         // destination mode
  pci_conf_write(f, f->msi_capreg + 4*2, 0);
  
  // Step 5 and 6. Allocate messages for the device.  Since we
  // support only one message and that is the default value in
  // the message control register, we do nothing.

  // Step 7. Write base message data pattern into the device's
  // Message Data Register.
  // (The Message Data Register format is mandated by the x86
  // architecture.  See 9.11.2 in the Vol. 3 of the Intel architecture
  // manual.
  pci_conf_write(f, f->msi_capreg + 4*3,
                 (0 << 15) |        // trigger mode (edge)
                 //(0 << 14) |      // level for trigger mode (don't care)
                 (0 << 8) |         // delivery mode (fixed)
                 (irqnum+T_IRQ0));  // vector

  // Step 8. Set the MSI enable bit in the device's Message
  // control register.
  pci_conf_write(f, f->msi_capreg, cap_entry | (1 << 16));
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

      u32 cmd_status = pci_conf_read(&af, PCI_COMMAND_STATUS_REG);
      if (cmd_status & PCI_STATUS_CAPLIST_SUPPORT)
        pci_scan_caplist(&af);

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
  u32 busreg = pci_conf_read(pcif, PCI_BRIDGE_BUS_REG);

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

  if (VERBOSE)
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
