// A second Intel 8250 serial port (UART) for early-boot debug output.
// Requires that QEMU was started with a second serial port, otherwise
// debug output is suppressed.
//
// We want to allow debug output as early as possible and from anywhere,
// including the spinlock code. This requires a custom lock to make sure
// output is complete and consistent. Our custom lock cannot possibly be
// acquired from an interrupt context so it doesn't disable interrupts.

#include "types.h"
#include "defs.h"
#include "x86.h"
#include "uart.h"

static int uart;  // do we have a second uart?
static volatile uint locked;  // custom lock

static void
lock(void)
{
  while(xchg(&locked, 1) != 0)
    ;
}

static void
unlock(void)
{
  xchg(&locked, 0);
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

  if(fmt == 0){
    debugf("debugf: null fmt\n");  // no panic(), no console.c dependency
    return;
  }

  lock();

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

  unlock();
}
