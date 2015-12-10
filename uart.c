// Intel 8250 serial port (UART).

#include "types.h"
#include "defs.h"
#include "traps.h"
#include "x86.h"
#include "uart.h"

static int uart;    // is there a uart?

void
uartinit(void)
{
  char *p;

  // Turn off the FIFO
  outb(COM1+UART_FIFO_CONTROL, 0);

  // 9600 baud, 8 data bits, 1 stop bit, parity off.
  outb(COM1+UART_LINE_CONTROL, UART_DIVISOR_LATCH);     // Unlock divisor
  outb(COM1+UART_DIVISOR_LOW, 115200/9600);
  outb(COM1+UART_DIVISOR_HIGH, 0);
  outb(COM1+UART_LINE_CONTROL, 0x03);   // Lock divisor, 8 data bits.
  outb(COM1+UART_MODEM_CONTROL, 0);

  // Enable receive interrupts.
  outb(COM1+UART_INTERRUPT_ENABLE, UART_RECEIVE_INTERRUPT);

  // If status is 0xFF, no serial port.
  if(inb(COM1+UART_LINE_STATUS) == 0xFF)
    return;
  uart = 1;

  // Acknowledge pre-existing interrupt conditions;
  // enable interrupts.
  inb(COM1+UART_INTERRUPT_ID);
  inb(COM1+UART_RECEIVE_BUFFER);
  picenable(IRQ_COM1);
  ioapicenable(IRQ_COM1, 0);

  // Announce that we're here.
  for(p="xv6...\n"; *p; p++)
    uartputc(*p);
}

static int
cantransmit(void)
{
  return inb(COM1+UART_LINE_STATUS) & UART_TRANSMIT_READY;
}

static int
hasreceived(void)
{
  return inb(COM1+UART_LINE_STATUS) & UART_RECEIVE_READY;
}

void
uartputc(int c)
{
  int i;

  if(!uart)
    return;
  for(i = 0; i < 128 && !cantransmit(); i++)
    microdelay(10);
  outb(COM1+UART_TRANSMIT_BUFFER, c);
}

static int
uartgetc(void)
{
  if(!uart)
    return -1;
  if(!hasreceived())
    return -1;
  return inb(COM1+UART_RECEIVE_BUFFER);
}

void
uartintr(void)
{
  consoleintr(uartgetc);
}
