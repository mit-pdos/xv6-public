#include <types.h>
#include <x86.h>
#include "defs.h"
#include "spinlock.h"

struct spinlock console_lock;
int use_printf_lock = 0;

/*
 * copy console output to parallel port, which you can tell
 * .bochsrc to copy to the stdout:
 * parport1: enabled=1, file="/dev/stdout"
 */
static void
lpt_putc(int c)
{
	int i;

	for (i = 0; !(inb(0x378+1) & 0x80) && i < 12800; i++)
		;
	outb(0x378+0, c);
	outb(0x378+2, 0x08|0x04|0x01);
	outb(0x378+2, 0x08);
}

void
cons_putc(int c)
{
  int crtport = 0x3d4; // io port of CGA
  unsigned short *crt = (unsigned short *) 0xB8000; // base of CGA memory
  int ind;

  if(use_printf_lock)
    acquire(&console_lock);

  lpt_putc(c);

  // cursor position, 16 bits, col + 80*row
  outb(crtport, 14);
  ind = inb(crtport + 1) << 8;
  outb(crtport, 15);
  ind |= inb(crtport + 1);

  c &= 0xff;

  if(c == '\n'){
    ind -= (ind % 80);
    ind += 80;
  } else {
    c |= 0x0700; // black on white
    crt[ind] = c;
    ind += 1;
  }

  if((ind / 80) >= 24){
    // scroll up
    memcpy(crt, crt + 80, sizeof(crt[0]) * (23 * 80));
    ind -= 80;
    memset(crt + ind, 0, sizeof(crt[0]) * ((24 * 80) - ind));
  }

  outb(crtport, 14);
  outb(crtport + 1, ind >> 8);
  outb(crtport, 15);
  outb(crtport + 1, ind);

  if(use_printf_lock)
    release(&console_lock);
}

void
printint(int xx, int base, int sgn)
{
  char buf[16];
  char digits[] = "0123456789ABCDEF";
  int i = 0, neg = 0;
  unsigned int x;
  
  if(sgn && xx < 0){
    neg = 1;
    x = 0 - xx;
  } else {
    x = xx;
  }

  do {
    buf[i++] = digits[x % base];
  } while((x /= base) != 0);
  if(neg)
    buf[i++] = '-';

  while(i > 0){
    i -= 1;
    cons_putc(buf[i]);
  }
}

/*
 * print to the console. only understands %d and %x.
 */
void
cprintf(char *fmt, ...)
{
  int i, state = 0, c;
  unsigned int *ap = (unsigned int *) &fmt + 1;

  for(i = 0; fmt[i]; i++){
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
        state = '%';
      } else {
        cons_putc(c);
      }
    } else if(state == '%'){
      if(c == 'd'){
        printint(*ap, 10, 1);
        ap++;
      } else if(c == 'x' || c == 'p'){
        printint(*ap, 16, 0);
        ap++;
      } else if(c == '%'){
        cons_putc(c);
      }
      state = 0;
    }
  }
}

void
panic(char *s)
{
  use_printf_lock = 0;
  cprintf("panic: ");
  cprintf(s, 0);
  cprintf("\n", 0);
  while(1)
    ;
}
