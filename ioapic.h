#define IO_APIC_BASE   0xFEC00000   // Default phys addr of IO APIC
#define IOAPIC_WINDOW        0x10   // Window register offset

// Constants relating to APIC ID registers
#define APIC_ID_MASK            0xff000000
#define APIC_ID_SHIFT           24
#define APIC_ID_CLUSTER         0xf0
#define APIC_ID_CLUSTER_ID      0x0f
#define APIC_MAX_CLUSTER        0xe
#define APIC_MAX_INTRACLUSTER_ID 3
#define APIC_ID_CLUSTER_SHIFT   4

// Fields in VER
#define APIC_VER_VERSION        0x000000ff
#define APIC_VER_MAXLVT         0x00ff0000
#define MAXLVTSHIFT             16

// Indexes into IO APIC
#define IOAPIC_ID               0x00
#define IOAPIC_VER              0x01
#define IOAPIC_ARB              0x02
#define IOAPIC_REDTBL           0x10
#define IOAPIC_REDTBL0          IOAPIC_REDTBL
#define IOAPIC_REDTBL1          (IOAPIC_REDTBL+0x02)
#define IOAPIC_REDTBL2          (IOAPIC_REDTBL+0x04)
#define IOAPIC_REDTBL3          (IOAPIC_REDTBL+0x06)
#define IOAPIC_REDTBL4          (IOAPIC_REDTBL+0x08)
#define IOAPIC_REDTBL5          (IOAPIC_REDTBL+0x0a)
#define IOAPIC_REDTBL6          (IOAPIC_REDTBL+0x0c)
#define IOAPIC_REDTBL7          (IOAPIC_REDTBL+0x0e)
#define IOAPIC_REDTBL8          (IOAPIC_REDTBL+0x10)
#define IOAPIC_REDTBL9          (IOAPIC_REDTBL+0x12)
#define IOAPIC_REDTBL10         (IOAPIC_REDTBL+0x14)
#define IOAPIC_REDTBL11         (IOAPIC_REDTBL+0x16)
#define IOAPIC_REDTBL12         (IOAPIC_REDTBL+0x18)
#define IOAPIC_REDTBL13         (IOAPIC_REDTBL+0x1a)
#define IOAPIC_REDTBL14         (IOAPIC_REDTBL+0x1c)
#define IOAPIC_REDTBL15         (IOAPIC_REDTBL+0x1e)
#define IOAPIC_REDTBL16         (IOAPIC_REDTBL+0x20)
#define IOAPIC_REDTBL17         (IOAPIC_REDTBL+0x22)
#define IOAPIC_REDTBL18         (IOAPIC_REDTBL+0x24)
#define IOAPIC_REDTBL19         (IOAPIC_REDTBL+0x26)
#define IOAPIC_REDTBL20         (IOAPIC_REDTBL+0x28)
#define IOAPIC_REDTBL21         (IOAPIC_REDTBL+0x2a)
#define IOAPIC_REDTBL22         (IOAPIC_REDTBL+0x2c)
#define IOAPIC_REDTBL23         (IOAPIC_REDTBL+0x2e)

// Fields in the IO APIC's redirection table entries
#define IOART_DEST      APIC_ID_MASK    // broadcast addr: all APICs

#define IOART_RESV      0x00fe0000      // reserved

#define IOART_INTMASK   0x00010000      // R/W: INTerrupt mask
#define IOART_INTMCLR   0x00000000      //       clear, allow INTs
#define IOART_INTMSET   0x00010000      //       set, inhibit INTs

#define IOART_TRGRMOD   0x00008000      // R/W: trigger mode
#define IOART_TRGREDG   0x00000000      //       edge
#define IOART_TRGRLVL   0x00008000      //       level

#define IOART_REM_IRR   0x00004000      // RO: remote IRR

#define IOART_INTPOL    0x00002000      // R/W: INT input pin polarity
#define IOART_INTAHI    0x00000000      //      active high
#define IOART_INTALO    0x00002000      //      active low

#define IOART_DELIVS    0x00001000      // RO: delivery status

#define IOART_DESTMOD   0x00000800      // R/W: destination mode
#define IOART_DESTPHY   0x00000000      //      physical
#define IOART_DESTLOG   0x00000800      //      logical

#define IOART_DELMOD    0x00000700      // R/W: delivery mode
#define IOART_DELFIXED  0x00000000      //       fixed
#define IOART_DELLOPRI  0x00000100      //       lowest priority
#define IOART_DELSMI    0x00000200      //       System Management INT
#define IOART_DELRSV1   0x00000300      //       reserved
#define IOART_DELNMI    0x00000400      //       NMI signal
#define IOART_DELINIT   0x00000500      //       INIT signal
#define IOART_DELRSV2   0x00000600      //       reserved
#define IOART_DELEXINT  0x00000700      //       External INTerrupt

#define IOART_INTVEC    0x000000ff      // R/W: INTerrupt vector field

// Fields in VER
#define IOART_VER_VERSION       0x000000ff
#define IOART_VER_MAXREDIR      0x00ff0000
#define MAXREDIRSHIFT           16
