// Console input and output.
// Input is from the keyboard only.
// Output is written to the screen and the printer port.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "dev.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"

#define CRTPORT 0x3d4
#define LPTPORT 0x378
#define BACKSPACE 0x100

static ushort *crt = (ushort*)0xb8000;  // CGA memory

static struct spinlock console_lock;
int panicked = 0;
int use_console_lock = 0;

// Copy console output to parallel port, which you can tell
// .bochsrc to copy to the stdout:
//   parport1: enabled=1, file="/dev/stdout"
static void
lpt_putc(int c)
{
  int i;

  for(i = 0; !(inb(LPTPORT+1) & 0x80) && i < 12800; i++)
    ;
  if(c == BACKSPACE)
    c = '\b';
  outb(LPTPORT+0, c);
  outb(LPTPORT+2, 0x08|0x04|0x01);
  outb(LPTPORT+2, 0x08);
}

static void
cga_putc(int c)
{
  int pos;
  
  // Cursor position: col + 80*row.
  outb(CRTPORT, 14);
  pos = inb(CRTPORT+1) << 8;
  outb(CRTPORT, 15);
  pos |= inb(CRTPORT+1);

  if(c == '\n')
    pos += 80 - pos%80;
  else if(c == BACKSPACE){
    if(pos > 0)
      crt[--pos] = ' ' | 0x0700;
  } else
    crt[pos++] = (c&0xff) | 0x0700;  // black on white
  
  if((pos/80) >= 24){  // Scroll up.
    memmove(crt, crt+80, sizeof(crt[0])*23*80);
    pos -= 80;
    memset(crt+pos, 0, sizeof(crt[0])*(24*80 - pos));
  }
  
  outb(CRTPORT, 14);
  outb(CRTPORT+1, pos>>8);
  outb(CRTPORT, 15);
  outb(CRTPORT+1, pos);
  crt[pos] = ' ' | 0x0700;
}

void
cons_putc(int c)
{
  if(panicked){
    cli();
    for(;;)
      ;
  }

  lpt_putc(c);
  cga_putc(c);
}

void
printint(int xx, int base, int sgn)
{
  static char digits[] = "0123456789ABCDEF";
  char buf[16];
  int i = 0, neg = 0;
  uint x;

  if(sgn && xx < 0){
    neg = 1;
    x = 0 - xx;
  } else {
    x = xx;
  }

  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);
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

int
console_write(struct inode *ip, char *buf, int n)
{
  int i;

  iunlock(ip);
  acquire(&console_lock);
  for(i = 0; i < n; i++)
    cons_putc(buf[i] & 0xff);
  release(&console_lock);
  ilock(ip);

  return n;
}

#define INPUT_BUF 128
struct {
  struct spinlock lock;
  char buf[INPUT_BUF];
  uint r;  // Read index
  uint w;  // Write index
  uint e;  // Edit index
} input;

#define C(x)  ((x)-'@')  // Control-x

void
console_intr(int (*getc)(void))
{
  int c;

  acquire(&input.lock);
  while((c = getc()) >= 0){
    switch(c){
    case C('P'):  // Process listing.
      procdump();
      break;
    case C('U'):  // Kill line.
      while(input.e != input.w &&
            input.buf[(input.e-1) % INPUT_BUF] != '\n'){
        input.e--;
        cons_putc(BACKSPACE);
      }
      break;
    case C('H'):  // Backspace
      if(input.e != input.w){
        input.e--;
        cons_putc(BACKSPACE);
      }
      break;
    default:
      if(c != 0 && input.e-input.r < INPUT_BUF){
        input.buf[input.e++ % INPUT_BUF] = c;
        cons_putc(c);
        if(c == '\n' || c == C('D') || input.e == input.r+INPUT_BUF){
          input.w = input.e;
          wakeup(&input.r);
        }
      }
      break;
    }
  }
  release(&input.lock);
}

int
console_read(struct inode *ip, char *dst, int n)
{
  uint target;
  int c;

  iunlock(ip);
  target = n;
  acquire(&input.lock);
  while(n > 0){
    while(input.r == input.w){
      if(cp->killed){
        release(&input.lock);
        ilock(ip);
        return -1;
      }
      sleep(&input.r, &input.lock);
    }
    c = input.buf[input.r++ % INPUT_BUF];
    if(c == C('D')){  // EOF
      if(n < target){
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        input.r--;
      }
      break;
    }
    *dst++ = c;
    --n;
    if(c == '\n')
      break;
  }
  release(&input.lock);
  ilock(ip);

  return target - n;
}

void
console_init(void)
{
  initlock(&console_lock, "console");
  initlock(&input.lock, "console input");

  devsw[CONSOLE].write = console_write;
  devsw[CONSOLE].read = console_read;
  use_console_lock = 1;

  pic_enable(IRQ_KBD);
  ioapic_enable(IRQ_KBD, 0);
}

void
panic(char *s)
{
  int i;
  uint pcs[10];
  
  __asm __volatile("cli");
  use_console_lock = 0;
  cprintf("cpu%d: panic: ", cpu());
  cprintf(s);
  cprintf("\n");
  getcallerpcs(&s, pcs);
  for(i=0; i<10; i++)
    cprintf(" %p", pcs[i]);
  panicked = 1; // freeze other CPU
  for(;;)
    ;
}

