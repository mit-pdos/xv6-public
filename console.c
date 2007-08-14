#include "types.h"
#include "x86.h"
#include "traps.h"
#include "defs.h"
#include "spinlock.h"
#include "dev.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "kbd.h"

struct spinlock console_lock;
int panicked = 0;
int use_console_lock = 0;

// Copy console output to parallel port, which you can tell
// .bochsrc to copy to the stdout:
//   parport1: enabled=1, file="/dev/stdout"
static void
lpt_putc(int c)
{
  int i;

  for(i = 0; !(inb(0x378+1) & 0x80) && i < 12800; i++)
    ;
  outb(0x378+0, c);
  outb(0x378+2, 0x08|0x04|0x01);
  outb(0x378+2, 0x08);
}

static void
cons_putc(int c)
{
  int crtport = 0x3d4; // io port of CGA
  ushort *crt = (ushort*) 0xB8000; // base of CGA memory
  int ind;

  if(panicked){
    cli();
    for(;;)
      ;
  }

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
    ind++;
  }

  if((ind / 80) >= 24){
    // scroll up
    memmove(crt, crt + 80, sizeof(crt[0]) * (23 * 80));
    ind -= 80;
    memset(crt + ind, 0, sizeof(crt[0]) * ((24 * 80) - ind));
  }

  outb(crtport, 14);
  outb(crtport + 1, ind >> 8);
  outb(crtport, 15);
  outb(crtport + 1, ind);
}

void
printint(int xx, int base, int sgn)
{
  char buf[16];
  char digits[] = "0123456789ABCDEF";
  int i = 0, neg = 0;
  uint x;

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

  while(--i >= 0)
    cons_putc(buf[i]);
}

// Print to the console. only understands %d, %x, %p, %s.
void
cprintf(char *fmt, ...)
{
  int i, c, state, locking;
  uint *argp;
  char *s;

  locking = use_console_lock;
  if(locking)
    acquire(&console_lock);

  argp = (uint*)(void*)&fmt + 1;
  state = 0;
  for(i = 0; fmt[i]; i++){
    c = fmt[i] & 0xff;
    switch(state){
    case 0:
      if(c == '%')
        state = '%';
      else
        cons_putc(c);
      break;
    
    case '%':
      switch(c){
      case 'd':
        printint(*argp++, 10, 1);
        break;
      case 'x':
      case 'p':
        printint(*argp++, 16, 0);
        break;
      case 's':
        s = (char*)*argp++;
        if(s == 0)
          s = "(null)";
        for(; *s; s++)
          cons_putc(*s);
        break;
      case '%':
        cons_putc('%');
        break;
      default:
        // Print unknown % sequence to draw attention.
        cons_putc('%');
        cons_putc(c);
        break;
      }
      state = 0;
      break;
    }
  }

  if(locking)
    release(&console_lock);
}

void
panic(char *s)
{
  int i;
  uint pcs[10];
  
  __asm __volatile("cli");
  use_console_lock = 0;
  cprintf("panic (%d): ", cpu());
  cprintf(s, 0);
  cprintf("\n", 0);
  getcallerpcs(&s, pcs);
  for(i=0; i<10; i++)
    cprintf(" %p", pcs[i]);
  panicked = 1; // freeze other CPU
  for(;;)
    ;
}

int
console_write(int minor, char *buf, int n)
{
  int i;

  acquire(&console_lock);
  for(i = 0; i < n; i++)
    cons_putc(buf[i] & 0xff);
  release(&console_lock);

  return n;
}

#define KBD_BUF 64
struct {
  uchar buf[KBD_BUF];
  int r;
  int w;
  struct spinlock lock;
} kbd;

void
kbd_intr(void)
{
  static uint shift;
  static uchar *charcode[4] = {
    normalmap,
    shiftmap,
    ctlmap,
    ctlmap
  };
  uint st, data, c;

  acquire(&kbd_lock);

  st = inb(KBSTATP);
  if((st & KBS_DIB) == 0)
    goto out;
  data = inb(KBDATAP);

  if(data == 0xE0) {
    shift |= E0ESC;
    goto out;
  } else if(data & 0x80) {
    // Key released
    data = (shift & E0ESC ? data : data & 0x7F);
    shift &= ~(shiftcode[data] | E0ESC);
    goto out;
  } else if(shift & E0ESC) {
    // Last character was an E0 escape; or with 0x80
    data |= 0x80;
    shift &= ~E0ESC;
  }

  shift |= shiftcode[data];
  shift ^= togglecode[data];

  c = charcode[shift & (CTL | SHIFT)][data];
  if(shift & CAPSLOCK) {
    if('a' <= c && c <= 'z')
      c += 'A' - 'a';
    else if('A' <= c && c <= 'Z')
      c += 'a' - 'A';
  }

  switch(c){
  case 0:
    // Ignore unknown keystrokes.
    break;
  
  case C('T'):
    cprintf("#");  // Let user know we're still alive.
    break;
  
  case C('P'):
    procdump();
    break;

  default:
    if(((kbd_w + 1) % KBD_BUF) != kbd_r){
      kbd_buf[kbd_w++] = c;
      if(kbd_w >= KBD_BUF)
        kbd_w = 0;
      wakeup(&kbd_r);
    }
    break;
  }

out:
  release(&kbd_lock);
}

//PAGEBREAK: 25
int
console_read(int minor, char *dst, int n)
{
  uint target;
  int c;

  target = n;
  acquire(&kbd.lock);
  while(n > 0){
    while(kbd.r == kbd.w){
      if(cp->killed){
        release(&kbd.lock);
        return -1;
      }
      sleep(&kbd.r, &kbd.lock);
    }
    c = kbd.buf[kbd.r++];
    if(c == C('D')){  // EOF
      if(n < target){
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        kbd.r--;
      }
      break;
    }
    *dst++ = c;
    cons_putc(c);
    --n;
    if(kbd.r >= KBD_BUF)
      kbd.r = 0;
  }
  release(&kbd.lock);

  return target - n;
}

void
console_init(void)
{
  initlock(&console_lock, "console");
  initlock(&kbd.lock, "kbd");

  devsw[CONSOLE].write = console_write;
  devsw[CONSOLE].read = console_read;
  use_console_lock = 1;

  irq_enable(IRQ_KBD);
  ioapic_enable(IRQ_KBD, 0);
}

