// Intel 8253/8254/82C54 Programmable Interval Timer (PIT).
// http://en.wikipedia.org/wiki/Intel_8253

#include "types.h"
#include "amd64.h"

#define IO_TIMER1       0x040           // 8253 Timer #1
#define TIMER_FREQ      1193182
#define	TIMER_CNTR      (IO_TIMER1 + 0)	// timer counter port
#define TIMER_MODE      (IO_TIMER1 + 3) // timer mode port
#define TIMER_SEL0      0x00    // select counter 0
#define TIMER_TCOUNT    0x00    // mode 0, terminal count
#define TIMER_16BIT     0x30    // r/w counter 16 bits, LSB first
#define TIMER_STAT      0xe0    // read status mode
#define TIMER_STAT0     (TIMER_STAT | 0x2)  // status mode counter 0

u64 cpuhz;

void
microdelay(u64 delay)
{
  u64 tscdelay = (cpuhz * delay) / 1000000;
  u64 s = rdtsc();
  while (rdtsc() - s < tscdelay)
    nop_pause();
}

void
inithz(void)
{
  // Setup PIT for terminal count starting from 2^16 - 1
  u64 ticks = 0x000000000000FFFFull;
  outb(TIMER_MODE, TIMER_SEL0 | TIMER_TCOUNT | TIMER_16BIT);  
  outb(IO_TIMER1, ticks % 256);
  outb(IO_TIMER1, ticks / 256);

  // Wait until OUT bit of status byte is set
  u64 s = rdtsc();
  do {
    outb(TIMER_MODE, TIMER_STAT0);
  } while (!(inb(TIMER_CNTR) & 0x80));
  u64 e = rdtsc();

  cpuhz = ((e-s)*10000000) / ((ticks*10000000)/TIMER_FREQ);
}
