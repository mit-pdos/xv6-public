// Access is through these two I/O addresses. Select a register by writing to
// CMOS_PORT, then read/write data from/to that register through CMOS_RETURN.
//
// Note that due to general IBM insanity bit 7 of CMOS_PORT controls whether
// NMIs get to "the" CPU or not. All of xv6 has ignored this issue for years,
// so we do the same. But this technically makes CMOS_PORT a 7-bit address
// and the CMOS cannot have more than 128 registers. The original Motorola
// MC146818 in fact had 64 registers (only AD0-AD5 were used for addresses).
#define CMOS_PORT    0x70
  #define CMOS_NMI_BIT    (1 << 7)  // NMI enabled
#define CMOS_RETURN  0x71

// The CMOS registers for the real-time clock.
#define CMOS_SECS    0x00
#define CMOS_MINS    0x02
#define CMOS_HOURS   0x04
  #define CMOS_PM_BIT     (1 << 7)  // RTC PM
#define CMOS_DAY     0x07
#define CMOS_MONTH   0x08
#define CMOS_YEAR    0x09
#define CMOS_STATA   0x0a
  #define CMOS_UIP_BIT    (1 << 7)  // RTC update in progress
#define CMOS_STATB   0x0b
  #define CMOS_24H_BIT    (1 << 1)  // RTC 24/12 hour format
  #define CMOS_BINARY_BIT (1 << 2)  // RTC binary/bcd format

// The shutdown status register is used in lapic.c to bring up APs.
#define CMOS_SHUTDOWN_STAT 0x0f
  #define CMOS_JMP_DWORD_NO_EOI 0x0a
