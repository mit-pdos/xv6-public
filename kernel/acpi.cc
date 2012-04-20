// http://www.acpi.info/spec.htm

#include "types.h"
#include "amd64.h"
#include "kernel.hh"
#include "cpu.hh"
#include "apic.hh"

struct rsdp {
  u8 signature[8];
  u8 checksum;
  u8 oemid[6];
  u8 revision;
  u32 rsdtaddr;
  u32 length;
  u64 xsdtaddr;
  u8 extchecksum;
  u8 reserved[3];
};

struct header {
  u8 signature[4];
  u32 length;
  u8 revision;
  u8 checksum;
  u8 oemid[6];
  u8 oemtableid[8];
  u32 oemrevision;
  u32 creatorid;
  u32 creatorrevision;
};

struct rsdt {
  struct header hdr;
  u32 entry[];
};

struct xsdt {
  struct header hdr;
  u64 entry[];
} __attribute__((packed));

struct madt {
  struct header hdr;
  u32 localaddr;
  u32 flags;
  u8 entry[];
};

struct madt_apic {
  u8 type;
  u8 length;
  u8 procid;
  u8 apicid;
  u32 flags;
} __attribute__((packed));

struct madt_x2apic {
  u8 type;
  u8 length;
  u8 reserved[2];
  u32 apicid;
  u32 flags;
  u32 procuid;
} __attribute__((packed));

#define CPU_ENABLED 0x1

static u8
sum(u8 *a, u32 length)
{
    u8 s = 0;
    for (u32 i = 0; i < length; i++)
	s += a[i];
    return s;
}

static struct rsdp *
rsdp_search1(paddr pa, int len)
{
    u8 *start = (u8 *)p2v(pa);
    for (u8 *p = start; p < (start + len); p += 16) {
	if ((memcmp(p, "RSD PTR ", 8) == 0) && (sum(p, 20) == 0))
	    return (struct rsdp *)p;
    }
    return 0;
}

static struct rsdp *
rsdp_search(void)
{
    struct rsdp *ret;
    u8 *bda;
    paddr pa;
    
    bda = (u8 *)p2v(0x400);
    if ((pa = ((bda[0x0F] << 8) | bda[0x0E]) << 4)) {
	if ((ret = rsdp_search1(pa, 1024)))
	    return ret;
    } 
    return rsdp_search1(0xE0000, 0x20000);
}

static void
scan_madt(struct madt* madt)
{
  struct madt_x2apic* mx2;
  struct madt_apic* ma;
  u8* type;
  u8* end;
  u32 c;

  end = ((u8*)madt) + madt->hdr.length;
  type = ((u8*)madt) + sizeof(*madt);
  c = 0 == myid() ? 1 : 0;

  while (type < end) {
    s64 id = -1;

    switch (type[0]) {
    case 0: // Processor Local APIC
      ma = (struct madt_apic*) type;
      if (ma->flags & CPU_ENABLED)
        id = ma->apicid;
      break;
    case 9: // Processor Local x2APIC
      mx2 = (struct madt_x2apic*) type;
      if (mx2->flags & CPU_ENABLED)
        id = mx2->apicid;
      break;
    }

    if (id != -1 && id != lapicid().num) {
      assert(c < NCPU);
      if (VERBOSE)
        cprintf("%u from %u to %ld\n", c, cpus[c].hwid.num, id);
      cpus[c].hwid.num = id;
      c = c+1 == myid() ? c+2 : c+1;
    }

    type = type + type[1];
  }

}

void
initacpi(void)
{
  struct rsdp* rsdp = rsdp_search();
  struct madt* madt = nullptr;
    
  if (!rsdp)
    return;

  if (rsdp->xsdtaddr) {
    struct xsdt* xsdt = (struct xsdt*) p2v(rsdp->xsdtaddr);
    if (sum((u8 *)xsdt, xsdt->hdr.length)) {
      cprintf("initacpi: bad xsdt checksum\n");
      return;
    }
    
    u32 n = xsdt->hdr.length > sizeof(*xsdt) ? 
      (xsdt->hdr.length - sizeof(*xsdt)) / 8 : 0;
    for (u32 i = 0; i < n; i++) {
      struct header* h = (struct header*) p2v(xsdt->entry[i]);
      if (memcmp(h->signature, "APIC", 4) == 0) {
        madt = (struct madt*) h;
        break;
      }
    }
  } else {
    struct rsdt* rsdt = (struct rsdt*) p2v(rsdp->rsdtaddr);
    if (sum((u8 *)rsdt, rsdt->hdr.length)) {
      cprintf("initacpi: bad rsdt checksum\n");
      return;
    }
    
    u32 n = rsdt->hdr.length > sizeof(*rsdt) ? 
      (rsdt->hdr.length - sizeof(*rsdt)) / 8 : 0;
    for (u32 i = 0; i < n; i++) {
      struct header* h = (struct header*) p2v(rsdt->entry[i]);
      if (memcmp(h->signature, "APIC", 4) == 0) {
        madt = (struct madt*) h;
        break;
      }
    }
  }
  
  if (madt != nullptr)
    scan_madt(madt);
}
