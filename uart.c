#include "memlayout.h"

//
// qemu -machine virt has a 16550a UART
// qemu/hw/riscv/virt.c
// http://byterunner.com/16550.html
//
// caller should lock.
//

// address of one of the registers
#define R(reg) ((unsigned int*)(UART0 + 4*(reg)))

void
uartinit(void)
{
  // disable interrupts
  *R(1) = 0x00;

  // special mode to set baud rate
  *R(3) = 0x80;

  // LSB for baud rate of 38.4K
  *R(0) = 0x03;

  // MSB for baud rate of 38.4K
  *R(1) = 0x00;

  // leave set-baud mode,
  // and set word length to 8 bits, no parity.
  *R(3) = 0x03;

  // reset and enable FIFOs.
  *R(2) = 0x07;
}

void
uartputc(int c)
{
  *R(0) = c;
}

static int
uartgetc(void)
{
}

void
uartintr(void)
{
}
