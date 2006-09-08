#include "types.h"
#include "x86.h"
#include "defs.h"
#include "traps.h"

// Register definitions for the Intel
// 8253/8254/82C54 Programmable Interval Timer (PIT).

#define IO_TIMER1       0x040           // 8253 Timer #1
#define IO_TIMER2       0x048           // 8253 Timer #2 (EISA only)

// Frequency of all three count-down timers; (TIMER_FREQ/freq) is the
// appropriate count to generate a frequency of freq hz.

#define TIMER_FREQ      1193182
#define TIMER_DIV(x)    ((TIMER_FREQ+(x)/2)/(x))

#define TIMER_CNTR0     (IO_TIMER1 + 0) // timer 0 counter port
#define TIMER_CNTR1     (IO_TIMER1 + 1) // timer 1 counter port
#define TIMER_CNTR2     (IO_TIMER1 + 2) // timer 2 counter port
#define TIMER_MODE      (IO_TIMER1 + 3) // timer mode port
#define TIMER_SEL0      0x00    // select counter 0
#define TIMER_SEL1      0x40    // select counter 1
#define TIMER_SEL2      0x80    // select counter 2
#define TIMER_INTTC     0x00    // mode 0, intr on terminal cnt
#define TIMER_ONESHOT   0x02    // mode 1, one shot
#define TIMER_RATEGEN   0x04    // mode 2, rate generator
#define TIMER_SQWAVE    0x06    // mode 3, square wave
#define TIMER_SWSTROBE  0x08    // mode 4, s/w triggered strobe
#define TIMER_HWSTROBE  0x0a    // mode 5, h/w triggered strobe
#define TIMER_LATCH     0x00    // latch counter for reading
#define TIMER_LSB       0x10    // r/w counter LSB
#define TIMER_MSB       0x20    // r/w counter MSB
#define TIMER_16BIT     0x30    // r/w counter 16 bits, LSB first
#define TIMER_BCD       0x01    // count in BCD

void
pit8253_timerinit(void)
{
  // initialize 8253 clock to interrupt 100 times/sec
  outb(TIMER_MODE, TIMER_SEL0 | TIMER_RATEGEN | TIMER_16BIT);
  outb(IO_TIMER1, TIMER_DIV(100) % 256);
  outb(IO_TIMER1, TIMER_DIV(100) / 256);
  irq_enable(IRQ_TIMER);
}
