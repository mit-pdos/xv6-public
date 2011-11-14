// Intel 8250 serial port (UART).
// http://en.wikibooks.org/wiki/Serial_Programming/8250_UART_Programming
#include "types.h"
#include "kernel.h"
#include "amd64.h"
#include "traps.h"

#define COM2    0x2f8
#define COM1    0x3f8

#define COM COM2
#define IRQ_COM IRQ_COM2

static int uart;    // is there a uart?

void
uartputc(char c)
{
  int i;

  if(!uart)
    return;
  for(i = 0; i < 128 && !(inb(COM+5) & 0x20); i++)
    microdelay(10);
  outb(COM+0, c);
}

static int
uartgetc(void)
{
  if(!uart)
    return -1;
  if(!(inb(COM+5) & 0x01))
    return -1;
  return inb(COM+0);
}

void
uartintr(void)
{
  consoleintr(uartgetc);
}

void
inituart(void)
{
  char *p;

  // Turn off the FIFO
  outb(COM+2, 0);
  
  // 19200 baud
  outb(COM+3, 0x80);    // Unlock divisor
  outb(COM+0, 115200/19200);
  outb(COM+1, 0);
  // 8 bits, one stop bit, no parity
  outb(COM+3, 0x03);    // Lock divisor, 8 data bits.
  outb(COM+1, 0x01);    // Enable receive interrupts.
  // Data terminal ready
  outb(COM+4, 0x0);

  // If status is 0xFF, no serial port.
  if(inb(COM+5) == 0xFF)
    return;
  uart = 1;

  // Acknowledge pre-existing interrupt conditions;
  // enable interrupts.
  inb(COM+2);
  inb(COM+0);
  picenable(IRQ_COM);
  ioapicenable(IRQ_COM, 0);

  // Announce that we're here.
  for(p="uart..\n"; *p; p++)
    uartputc(*p);
}
