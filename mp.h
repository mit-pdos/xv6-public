// See MultiProcessor Specification Version 1.[14]

struct mp {             // floating pointer
  u8 signature[4];           // "_MP_"
  u32 physaddr;               // phys addr of MP config table
  u8 length;                 // 1
  u8 specrev;                // [14]
  u8 checksum;               // all bytes must add up to 0
  u8 type;                   // MP system config type
  u8 imcrp;
  u8 reserved[3];
};

struct mpconf {         // configuration table header
  u8 signature[4];           // "PCMP"
  u16 length;                // total table length
  u8 version;                // [14]
  u8 checksum;               // all bytes must add up to 0
  u8 product[20];            // product id
  u32 oemtable;               // OEM table pointer
  u16 oemlength;             // OEM table length
  u16 entry;                 // entry count
  u32 lapicaddr;              // address of local APIC
  u16 xlength;               // extended table length
  u8 xchecksum;              // extended table checksum
  u8 reserved;
};

struct mpproc {         // processor table entry
  u8 type;                   // entry type (0)
  u8 apicid;                 // local APIC id
  u8 version;                // local APIC verison
  u8 flags;                  // CPU flags
    #define MPBOOT 0x02           // This proc is the bootstrap processor.
  u8 signature[4];           // CPU signature
  u32 feature;                 // feature flags from CPUID instruction
  u8 reserved[8];
};

struct mpioapic {       // I/O APIC table entry
  u8 type;                   // entry type (2)
  u8 apicno;                 // I/O APIC id
  u8 version;                // I/O APIC version
  u8 flags;                  // I/O APIC flags
  u32 addr;                  // I/O APIC address
};

// Table entry types
#define MPPROC    0x00  // One per processor
#define MPBUS     0x01  // One per bus
#define MPIOAPIC  0x02  // One per I/O APIC
#define MPIOINTR  0x03  // One per bus interrupt source
#define MPLINTR   0x04  // One per system interrupt source

