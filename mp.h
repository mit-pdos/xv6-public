// See MultiProcessor Specification Version 1.[14].

struct mp {             // floating pointer
  uchar signature[4];           // "_MP_"
  void *physaddr;               // phys addr of MP config table
  uchar length;                 // 1
  uchar specrev;                // [14]
  uchar checksum;               // all bytes must add up to 0
  uchar type;                   // MP system config type
  uchar imcrp;
  uchar reserved[3];
};

struct mpctb {          // configuration table header
  uchar signature[4];           // "PCMP"
  ushort length;                // total table length
  uchar version;                // [14]
  uchar checksum;               // all bytes must add up to 0
  uchar product[20];            // product id
  uint *oemtable;               // OEM table pointer
  ushort oemlength;             // OEM table length
  ushort entry;                 // entry count
  uint *lapicaddr;              // address of local APIC
  ushort xlength;               // extended table length
  uchar xchecksum;              // extended table checksum
  uchar reserved;
};

struct mppe {           // processor table entry
  uchar type;                   // entry type (0)
  uchar apicid;                 // local APIC id
  uchar version;                // local APIC verison
  uchar flags;                  // CPU flags
  uchar signature[4];           // CPU signature
  uint feature;                 // feature flags from CPUID instruction
  uchar reserved[8];
};

struct mpbe {           // bus table entry
  uchar type;                   // entry type (1)
  uchar busno;                  // bus id
  char string[6];               // bus type string
};

struct mpioapic {       // I/O APIC table entry
  uchar type;                   // entry type (2)
  uchar apicno;                 // I/O APIC id
  uchar version;                // I/O APIC version
  uchar flags;                  // I/O APIC flags
  uint *addr;                  // I/O APIC address
};

struct mpie {           // interrupt table entry
  uchar type;                   // entry type ([34])
  uchar intr;                   // interrupt type
  ushort flags;                 // interrupt flag
  uchar busno;                  // source bus id
  uchar irq;                    // source bus irq
  uchar apicno;                 // destination APIC id
  uchar intin;                  // destination APIC [L]INTIN#
};

enum {                  // table entry types
  MPPROCESSOR   = 0x00,         // one entry per processor
  MPBUS = 0x01,                 // one entry per bus
  MPIOAPIC = 0x02,              // one entry per I/O APIC
  MPIOINTR = 0x03,              // one entry per bus interrupt source
  MPLINTR = 0x04,               // one entry per system interrupt source

  MPSASM = 0x80,
  MPHIERARCHY   = 0x81,
  MPCBASM = 0x82,

                        // PCMPprocessor and PCMPioapic flags
  MPEN = 0x01,                  // enabled
  MPBP = 0x02,                  // bootstrap processor

                        // PCMPiointr and PCMPlintr flags
  MPPOMASK = 0x03,              // polarity conforms to bus specs
  MPHIGH = 0x01,                // active high
  MPLOW = 0x03,                 // active low
  MPELMASK = 0x0C,              // trigger mode of APIC input signals
  MPEDGE = 0x04,                // edge-triggered
  MPLEVEL = 0x0C,               // level-triggered

                        // PCMPiointr and PCMPlintr interrupt type
  MPINT = 0x00,                 // vectored interrupt from APIC Rdt
  MPNMI = 0x01,                 // non-maskable interrupt
  MPSMI = 0x02,                 // system management interrupt
  MPExtINT = 0x03,              // vectored interrupt from external PIC
};

// Common bits for
//      I/O APIC Redirection Table Entry;
//      Local APIC Local Interrupt Vector Table;
//      Local APIC Inter-Processor Interrupt;
//      Local APIC Timer Vector Table.
enum {
  APIC_FIXED     = 0x00000000,  // [10:8] Delivery Mode
  APIC_LOWEST    = 0x00000100,  // Lowest priority
  APIC_SMI       = 0x00000200,  // System Management Interrupt
  APIC_RR        = 0x00000300,  // Remote Read
  APIC_NMI       = 0x00000400,
  APIC_INIT      = 0x00000500,  // INIT/RESET
  APIC_STARTUP   = 0x00000600,  // Startup IPI
  APIC_EXTINT    = 0x00000700,

  APIC_PHYSICAL  = 0x00000000,  // [11] Destination Mode (RW)
  APIC_LOGICAL   = 0x00000800,

  APIC_DELIVS    = 0x00001000,  // [12] Delivery Status (RO)
  APIC_HIGH      = 0x00000000,  // [13] Interrupt Input Pin Polarity (RW)
  APIC_LOW       = 0x00002000,
  APIC_REMOTEIRR = 0x00004000,  // [14] Remote IRR (RO)
  APIC_EDGE      = 0x00000000,  // [15] Trigger Mode (RW)
  APIC_LEVEL     = 0x00008000,
  APIC_IMASK     = 0x00010000,  // [16] Interrupt Mask
};
