// Intel 8250 serial port (UART).
// http://en.wikibooks.org/wiki/Serial_Programming/8250_UART_Programming

#include "types.h"
#include "kernel.hh"
#include "amd64.h"
#include "traps.h"

#define COM2    0x2f8
#define COM1    0x3f8

static int com;
static int irq_com;
static int uart;    // is there a uart?

void
uartputc(char c)
{
  int i;

  if (!uart)
    return;
  for (i = 0; i < 128 && !(inb(com+5) & 0x20); i++)
    microdelay(10);
  outb(com+0, c);
}

static int
uartgetc(void)
{
  if (!uart)
    return -1;
  if (!(inb(com+5) & 0x01))
    return -1;
  return inb(com+0);
}

void
uartintr(void)
{
  consoleintr(uartgetc);
}

void
inituart(void)
{
  static struct {
    int com;
    int irq;
  } conf[] = {
    // Try COM2 (aka ttyS1) first, because it usually does SOL for IPMI.
    { COM2, IRQ_COM2 },
    // Still try COM1 (aka ttyS0), because it is what QEMU emulates.
    { COM1, IRQ_COM1 }
  };

  int i;
  for (i = 0; i < 2; i++) {
    com = conf[i].com;
    irq_com = conf[i].irq;

    // Turn off the FIFO
    outb(com+2, 0);
    // 19200 baud
    outb(com+3, 0x80);    // Unlock divisor
    outb(com+0, 115200/19200);
    outb(com+1, 0);
    // 8 bits, one stop bit, no parity
    outb(com+3, 0x03);    // Lock divisor, 8 data bits.
    outb(com+1, 0x01);    // Enable receive interrupts.
    // Data terminal ready
    outb(com+4, 0x0);
    
    // If status is 0xFF, no serial port.
    if(inb(com+5) != 0xFF)
      break;
  }
  if (i == 2)
    return;

  uart = 1;

  // Acknowledge pre-existing interrupt conditions;
  // enable interrupts.
  inb(com+2);
  inb(com+0);
  picenable(irq_com);
  ioapicenable(irq_com, 0);

  // Announce that we're here.
  for (const char *p=DEBUG?"xv6 DEBUG\n":"xv6\n"; *p; p++)
    uartputc(*p);
}
