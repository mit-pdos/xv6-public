#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

//
// qemu -machine virt has a 16550a UART
// qemu/hw/riscv/virt.c
// http://byterunner.com/16550.html
//
// caller should lock.
//

// address of one of the registers
#define R(reg) ((volatile unsigned char *)(UART0 + reg))

void
uartinit(void)
{
  // disable interrupts -- IER
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

  // reset and enable FIFOs -- FCR.
  *R(2) = 0x07;

  // enable receive interrupts -- IER.
  *R(1) = 0x01;
}

void
uartputc(int c)
{
  // wait for Transmit Holding Empty to be set in LSR.
  while((*R(5) & (1 << 5)) == 0)
    ;
  *R(0) = c;
}

int
uartgetc(void)
{
  if(*R(5) & 0x01){
    // input data is ready.
    return *R(0);
  } else {
    return -1;
  }
}

void
uartintr(void)
{
  while(1){
    int c = uartgetc();
    if(c == -1)
      break;
    consoleintr(c);
  }
}
