// References: ACPI 5.0 Errata A
// http://acpi.info/spec.htm

// 5.2.5.3
#define SIG_RDSP "RSD PTR "
struct acpi_rdsp {
  uchar signature[8];
  uchar checksum;
  uchar oem_id[6];
  uchar revision;
  uint32 rsdt_addr_phys;
  uint32 length;
  uint64 xsdt_addr_phys;
  uchar xchecksum;
  uchar reserved[3];
} __attribute__((__packed__));

// 5.2.6
struct acpi_desc_header {
  uchar signature[4];
  uint32 length;
  uchar revision;
  uchar checksum;
  uchar oem_id[6];
  uchar oem_tableid[8];
  uint32 oem_revision;
  uchar creator_id[4];
  uint32 creator_revision;
} __attribute__((__packed__));

// 5.2.7
struct acpi_rsdt {
  struct acpi_desc_header header;
  uint32 entry[0];
} __attribute__((__packed__));

#define TYPE_LAPIC 0
#define TYPE_IOAPIC 1
#define TYPE_INT_SRC_OVERRIDE 2
#define TYPE_NMI_INT_SRC 3
#define TYPE_LAPIC_NMI 4

// 5.2.12 Multiple APIC Description Table
#define SIG_MADT "APIC"
struct acpi_madt {
  struct acpi_desc_header header;
  uint32 lapic_addr_phys;
  uint32 flags;
  uchar table[0];
} __attribute__((__packed__));

// 5.2.12.2
#define APIC_LAPIC_ENABLED 1
struct madt_lapic {
  uchar type;
  uchar length;
  uchar acpi_id;
  uchar apic_id;
  uint32 flags;
} __attribute__((__packed__));

// 5.2.12.3
struct madt_ioapic {
  uchar type;
  uchar length;
  uchar id;
  uchar reserved;
  uint32 addr;
  uint32 interrupt_base;
} __attribute__((__packed__));
  

