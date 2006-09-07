#include "types.h"
#include "x86.h"
#include "traps.h"
#include "defs.h"
#include "spinlock.h"
#include "dev.h"
#include "param.h"
#include "mmu.h"

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
  int i, state = 0, c, locking = 0;
  uint *ap = (uint*)(void*)&fmt + 1;

  if(use_console_lock){
    locking = 1;
    acquire(&console_lock);
  }

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
      } else if(c == 's'){
        char *s = (char*)*ap;
        ap++;
        if(s == 0){
          cons_putc('0');
        }else{
          while(*s != 0){
            cons_putc(*s);
            s++;
          }
        }
      } else if(c == '%'){
        cons_putc(c);
      } else {
        // Unknown % sequence.  Print it to draw attention.
        cons_putc('%');
        cons_putc(c);
      }
      state = 0;
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

  for(i = 0; i < n; i++) {
    cons_putc(buf[i] & 0xff);
  }

  release(&console_lock);

  return n;
}


#define KBSTATP         0x64    // kbd controller status port(I)
#define KBS_DIB         0x01    // kbd data in buffer
#define KBDATAP         0x60    // kbd data port(I)

#define NO              0

#define SHIFT           (1<<0)
#define CTL             (1<<1)
#define ALT             (1<<2)

#define CAPSLOCK        (1<<3)
#define NUMLOCK         (1<<4)
#define SCROLLLOCK      (1<<5)

#define E0ESC           (1<<6)

// Special keycodes
#define KEY_HOME        0xE0
#define KEY_END         0xE1
#define KEY_UP          0xE2
#define KEY_DN          0xE3
#define KEY_LF          0xE4
#define KEY_RT          0xE5
#define KEY_PGUP        0xE6
#define KEY_PGDN        0xE7
#define KEY_INS         0xE8
#define KEY_DEL         0xE9

static uchar shiftcode[256] =
{
  [0x1D] CTL,
  [0x2A] SHIFT,
  [0x36] SHIFT,
  [0x38] ALT,
  [0x9D] CTL,
  [0xB8] ALT
};

static uchar togglecode[256] =
{
  [0x3A] CAPSLOCK,
  [0x45] NUMLOCK,
  [0x46] SCROLLLOCK
};

static uchar normalmap[256] =
{
  NO,   0x1B, '1',  '2',  '3',  '4',  '5',  '6',  // 0x00
  '7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t',
  'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',  // 0x10
  'o',  'p',  '[',  ']',  '\n', NO,   'a',  's',
  'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',  // 0x20
  '\'', '`',  NO,   '\\', 'z',  'x',  'c',  'v',
  'b',  'n',  'm',  ',',  '.',  '/',  NO,   '*',  // 0x30
  NO,   ' ',  NO,   NO,   NO,   NO,   NO,   NO,
  NO,   NO,   NO,   NO,   NO,   NO,   NO,   '7',  // 0x40
  '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
  '2',  '3',  '0',  '.',  NO,   NO,   NO,   NO,   // 0x50
  [0x97] KEY_HOME,
  [0x9C] '\n',      // KP_Enter
  [0xB5] '/',       // KP_Div
  [0xC8] KEY_UP,
  [0xC9] KEY_PGUP,
  [0xCB] KEY_LF,
  [0xCD] KEY_RT,
  [0xCF] KEY_END,
  [0xD0] KEY_DN,
  [0xD1] KEY_PGDN,
  [0xD2] KEY_INS,
  [0xD3] KEY_DEL
};

static uchar shiftmap[256] =
{
  NO,   033,  '!',  '@',  '#',  '$',  '%',  '^',  // 0x00
  '&',  '*',  '(',  ')',  '_',  '+',  '\b', '\t',
  'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',  // 0x10
  'O',  'P',  '{',  '}',  '\n', NO,   'A',  'S',
  'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',  // 0x20
  '"',  '~',  NO,   '|',  'Z',  'X',  'C',  'V',
  'B',  'N',  'M',  '<',  '>',  '?',  NO,   '*',  // 0x30
  NO,   ' ',  NO,   NO,   NO,   NO,   NO,   NO,
  NO,   NO,   NO,   NO,   NO,   NO,   NO,   '7',  // 0x40
  '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
  '2',  '3',  '0',  '.',  NO,   NO,   NO,   NO,   // 0x50
  [0x97] KEY_HOME,
  [0x9C] '\n',      // KP_Enter
  [0xB5] '/',       // KP_Div
  [0xC8] KEY_UP,
  [0xC9] KEY_PGUP,
  [0xCB] KEY_LF,
  [0xCD] KEY_RT,
  [0xCF] KEY_END,
  [0xD0] KEY_DN,
  [0xD1] KEY_PGDN,
  [0xD2] KEY_INS,
  [0xD3] KEY_DEL
};

#define C(x) (x - '@')

static uchar ctlmap[256] =
{
  NO,      NO,      NO,      NO,      NO,      NO,      NO,      NO,
  NO,      NO,      NO,      NO,      NO,      NO,      NO,      NO,
  C('Q'),  C('W'),  C('E'),  C('R'),  C('T'),  C('Y'),  C('U'),  C('I'),
  C('O'),  C('P'),  NO,      NO,      '\r',    NO,      C('A'),  C('S'),
  C('D'),  C('F'),  C('G'),  C('H'),  C('J'),  C('K'),  C('L'),  NO,
  NO,      NO,      NO,      C('\\'), C('Z'),  C('X'),  C('C'),  C('V'),
  C('B'),  C('N'),  C('M'),  NO,      NO,      C('/'),  NO,      NO,
  [0x97] KEY_HOME,
  [0xB5] C('/'),    // KP_Div
  [0xC8] KEY_UP,
  [0xC9] KEY_PGUP,
  [0xCB] KEY_LF,
  [0xCD] KEY_RT,
  [0xCF] KEY_END,
  [0xD0] KEY_DN,
  [0xD1] KEY_PGDN,
  [0xD2] KEY_INS,
  [0xD3] KEY_DEL
};

static uchar *charcode[4] = {
  normalmap,
  shiftmap,
  ctlmap,
  ctlmap
};

#define KBD_BUF 64
char kbd_buf[KBD_BUF];
int kbd_r;
int kbd_w;
struct spinlock kbd_lock;
static uint shift;

void
kbd_intr()
{
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

int
console_read(int minor, char *dst, int n)
{
  uint target = n;

  acquire(&kbd_lock);

  while(kbd_w == kbd_r) {
    sleep(&kbd_r, &kbd_lock);
  }

  while(n > 0 && kbd_w != kbd_r){
    *dst = (kbd_buf[kbd_r]) & 0xff;
    cons_putc(*dst & 0xff);
    dst++;
    --n;
    kbd_r++;
    if(kbd_r >= KBD_BUF)
      kbd_r = 0;
  }

  release(&kbd_lock);

  return target - n;
}

void
console_init()
{
  initlock(&console_lock, "console");
  initlock(&kbd_lock, "kbd");

  devsw[CONSOLE].write = console_write;
  devsw[CONSOLE].read = console_read;

  irq_enable(IRQ_KBD);
  ioapic_enable(IRQ_KBD, 0);

  use_console_lock = 1;
}
