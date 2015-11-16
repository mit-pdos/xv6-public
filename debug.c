// A second Intel 8250 serial port (UART) for early-boot debug output.
// Requires that QEMU was started with a second serial port, otherwise
// debug output is suppressed.
//
// We want to allow debug output as early as possible and from anywhere.
// This requires custom locking code to ensure our output is consistent,
// the existing spinlocks as well as pushcli/popcli cannot be used until
// seginit() has set up "cpu" correctly.

#include "types.h"
#include "defs.h"
#include "x86.h"
#include "mmu.h"
#include "uart.h"

static int uart;  // do we have a second uart?
static volatile uint locked;  // custom lock

// We use the intena parameter to remember if interrupts were enabled (or not)
// before we disabled them as part of taking the lock. The actual variable for
// this is in debugf() below. Because it lives on the stack of whoever called
// debugf(), we don't have to worry about which CPU we're on. The spinlocks in
// xv6 presumably pay that prize to make their interface simpler?

static void
lock(int *intena)
{
  *intena = readeflags() & FL_IF;
  // Migration is possible *right here*, but if so, FL_IF was asserted when
  // we sampled it and is still asserted now. That holds true up until the
  // CLI instruction itself. (Also see TRICKS.)
  cli();
  while(xchg(&locked, 1) != 0)
    ;
}

static void
unlock(int intena)
{
  xchg(&locked, 0);
  if(intena)
    sti();
}

static int
cantransmit(void)
{
  return inb(COM2+UART_LINE_STATUS) & UART_TRANSMIT_READY;
}

static void
debugputc(int c)
{
  int i;

  if(!uart)
    return;
  for(i = 0; i < 128 && !cantransmit(); i++)
    microdelay(10);
  outb(COM2+UART_TRANSMIT_BUFFER, c);
}

void
debuginit(void)
{
  // Turn off the FIFO
  outb(COM2+UART_FIFO_CONTROL, 0);

  // 9600 baud, 8 data bits, 1 stop bit, parity off.
  outb(COM2+UART_LINE_CONTROL, UART_DIVISOR_LATCH);
  outb(COM2+UART_DIVISOR_LOW, 115200/9600);
  outb(COM2+UART_DIVISOR_HIGH, 0);
  outb(COM2+UART_LINE_CONTROL, 0x03);   // 8 data bits.
  outb(COM2+UART_MODEM_CONTROL, 0);     // No RTS/DTR handshake.
  outb(COM2+UART_INTERRUPT_ENABLE, 0);  // Disable interrupts.

  // If status is 0xFF, no serial port.
  if(inb(COM2+UART_LINE_STATUS) == 0xFF)
    return;
  uart = 1;

  // Acknowledge pre-existing interrupt conditions, if any.
  // We don't use any of the interrupt stuff ourselves.
  inb(COM2+UART_INTERRUPT_ID);
  inb(COM2+UART_RECEIVE_BUFFER);
}

// It's certainly sad that we have to replicate the printf code *again* for
// this module. Alas there doesn't seem to be a nice way to modularize.

static void
printint(int xx, int base, int sign)
{
  static char digits[] = "0123456789abcdef";
  char buf[16];
  int i;
  uint x;

  if(sign && (sign = (xx < 0)))
    x = -xx;
  else
    x = xx;

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);

  if(sign)
    buf[i++] = '-';

  while(--i >= 0)
    debugputc(buf[i]);
}

void
debugf(char *fmt, ...)
{
  int i, c;
  uint *argp;
  char *s;
  int intena;

  if(fmt == 0){
    debugf("debugf: null fmt\n");  // no panic(), no console.c dependency
    return;
  }

  lock(&intena);

  argp = (uint*)(void*)(&fmt + 1);
  for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
    if(c != '%'){
      debugputc(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if(c == 0)
      break;
    switch(c){
    case 'd':
      printint(*argp++, 10, 1);
      break;
    case 'x':
    case 'p':
      printint(*argp++, 16, 0);
      break;
    case 's':
      if((s = (char*)*argp++) == 0)
        s = "(null)";
      for(; *s; s++)
        debugputc(*s);
      break;
    case '%':
      debugputc('%');
      break;
    default:
      // Print unknown % sequence to draw attention.
      debugputc('%');
      debugputc(c);
      break;
    }
  }

  unlock(intena);
}
