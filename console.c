// Console input and output.
// Input is from the keyboard or serial port.
// Output is written to the screen and serial port.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "pid_ns.h"
#include "proc.h"
#include "x86.h"
#include "fcntl.h"
#include "vfs_file.h"
#include "vfs_fs.h"
#include "kvector.h"

//PAGEBREAK: 50
#define BACKSPACE 0x100
#define CRTPORT 0x3d4
static ushort *crt = (ushort*)P2V(0xb8000);  // CGA memory
static int panicked = 0;

typedef struct device_lock {
  struct spinlock lock;
  int locking;
} device_lock;

typedef struct tty {
  int flags;
} tty;
tty tty_table[MAX_TTY];

static device_lock cons;

static void consputc(int);

static inline void update_pos(int pos) 
{
  outb(CRTPORT, 14);
  outb(CRTPORT+1, pos>>8);
  outb(CRTPORT, 15);
  outb(CRTPORT+1, pos);
  crt[pos] = ' ' | 0x0700;
}

static void
printint(int xx, int base, int sign)
{
  static char digits[] = "0123456789abcdef";
  char buf[16];
  int i;
  uint x;

  if(sign && (sign = xx < 0))
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
    consputc(buf[i]);
}
//PAGEBREAK: 50

// Print to the console. only understands %d, %x, %p, %s.
void
cprintf(char *fmt, ...)
{
  int i, c, locking;
  uint *argp;
  char *s;

  locking = cons.locking;
  if(locking)
    acquire(&cons.lock);

  if (fmt == 0)
    panic("null fmt");

  argp = (uint*)(void*)(&fmt + 1);
  for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
    if(c != '%'){
      consputc(c);
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
        consputc(*s);
      break;
    case 'c':{
      s = ((char*)*argp++);
      consputc(*s);
    }break;
    case '%':
      consputc('%');
      break;
    default:
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c);
      break;
    }
  }

  if(locking)
    release(&cons.lock);
}

void
panic(char *s)
{
  int i;
  uint pcs[10];

  cli();
  cons.locking = 0;
  // use lapiccpunum so that we can call panic from mycpu()
  cprintf("lapicid %d: panic: ", lapicid());
  cprintf(s);
  cprintf("\n");
  getcallerpcs(&s, pcs);
  for(i=0; i<10; i++)
    cprintf(" %p", pcs[i]);
  panicked = 1; // freeze other CPU
  for(;;)
    ;
}

static void
cgaputc(int c)
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
    if(pos > 0) --pos;
  } else
    crt[pos++] = (c&0xff) | 0x0700;  // black on white

  if(pos < 0 || pos > 25*80)
    panic("pos under/overflow");

  if((pos/80) >= 24){  // Scroll up.
    memmove(crt, crt+80, sizeof(crt[0])*23*80);
    pos -= 80;
    memset(crt+pos, 0, sizeof(crt[0])*(24*80 - pos));
  }

  update_pos(pos);
}

void consoleclear(void){
  int pos = 0;
  memset(crt, 0, sizeof(crt[0])*(24*80));
  update_pos(pos);
}

void
consputc(int c)
{
  if(panicked){
    cli();
    for(;;)
      ;
  }

  if(c == BACKSPACE){
    uartputc('\b'); uartputc(' '); uartputc('\b');
  } else
    uartputc(c);
  cgaputc(c);
}

#define INPUT_BUF 128
struct {
  char buf[INPUT_BUF];
  uint r;  // Read index
  uint w;  // Write index
  uint e;  // Edit index
} input;

#define C(x)  ((x)-'@')  // Control-x

void
consoleintr(int (*getc)(void))
{
  int c, doprocdump = 0;

  acquire(&cons.lock);
  while((c = getc()) >= 0){
    switch(c){
    case C('P'):  // Process listing.
      // procdump() locks cons.lock indirectly; invoke later
      doprocdump = 1;
      break;
    case C('U'):  // Kill line.
      while(input.e != input.w &&
            input.buf[(input.e-1) % INPUT_BUF] != '\n'){
        input.e--;
        consputc(BACKSPACE);
      }
      break;
    case C('H'): case '\x7f':  // Backspace
      if(input.e != input.w){
        input.e--;
        consputc(BACKSPACE);
      }
      break;
    default:
      if(c != 0 && input.e-input.r < INPUT_BUF){
        c = (c == '\r') ? '\n' : c;
        input.buf[input.e++ % INPUT_BUF] = c;
        consputc(c);
        if(c == '\n' || c == C('D') || input.e == input.r+INPUT_BUF){
          input.w = input.e;
          wakeup(&input.r);
        }
      }
      break;
    }
  }
  release(&cons.lock);
  if(doprocdump) {
    procdump();  // now call procdump() wo. cons.lock held
  }
}

int
consoleread(struct vfs_inode *ip, int n, vector * dstvector)
{
  uint target;
  int c;
  int dstindx = 0;
  ip->i_op.iunlock(ip);
  target = n;
  acquire(&cons.lock);
  while(n > 0){
    while(input.r == input.w){
      if(myproc()->killed){
        release(&cons.lock);
        ip->i_op.ilock(ip);
        return -1;
      }
      sleep(&input.r, &cons.lock);
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
    memmove_into_vector_bytes(*dstvector, dstindx++, (char*)&c, 1);
    --n;
    if(c == '\n')
      break;
  }
  release(&cons.lock);
  ip->i_op.ilock(ip);

  return target - n;
}

int
ttyread(struct vfs_inode *ip, int n, vector * dstvector)
{
  if(tty_table[ip->minor].flags & DEV_CONNECT){
    return consoleread(ip,n, dstvector);
  }
  return -1;
}

int
consolewrite(struct vfs_inode *ip, char *buf, int n)
{
  int i;

  ip->i_op.iunlock(ip);
  acquire(&cons.lock);
  for(i = 0; i < n; i++)
    consputc(buf[i] & 0xff);
  release(&cons.lock);
  ip->i_op.ilock(ip);

  return n;
}

int
ttywrite(struct vfs_inode *ip, char *buf, int n)
{
  if(tty_table[ip->minor].flags & DEV_CONNECT){
    return consolewrite(ip,buf,n);
  }
  //2DO: should return -1 when write to tty fails - filewrite panics.
  return n;
}

void
consoleinit(void)
{
  initlock(&cons.lock, "console");

  devsw[CONSOLE_MAJOR].write = ttywrite;
  devsw[CONSOLE_MAJOR].read = ttyread;
  tty_table[CONSOLE_MINOR].flags = DEV_CONNECT;

  cons.locking = 1;

  ioapicenable(IRQ_KBD, 0);
}

void
ttyinit(void)
{
  // we create tty devices after the console
  // therefor the tty's minor will be after the console's
  for(int i = CONSOLE_MINOR+1; i < MAX_TTY; i++){
     tty_table[i].flags = 0;
  }
}

void tty_disconnect(struct vfs_inode *ip) {
  tty_table[ip->minor].flags &=  ~(DEV_CONNECT);
  tty_table[CONSOLE_MINOR].flags |=  DEV_CONNECT;
  consoleclear();
  cprintf("Console connected\n");
}

void tty_connect(struct vfs_inode *ip) {
  tty_table[ip->minor].flags |= DEV_CONNECT;
  for(int i = CONSOLE_MINOR; i < MAX_TTY; i++){
    if(ip->minor != i){
      tty_table[i].flags &= ~(DEV_CONNECT);
  }
 }
 consoleclear();
 cprintf("\ntty%d connected\n",ip->minor-(CONSOLE_MINOR+1));
}

void tty_attach(struct vfs_inode *ip) {
  tty_table[ip->minor].flags |= DEV_ATTACH;
}

void tty_detach(struct vfs_inode *ip) {
  tty_table[ip->minor].flags &= ~(DEV_ATTACH);
}

int tty_gets(struct vfs_inode *ip, int command) {
  return (tty_table[ip->minor].flags & command ? 1 : 0);
}
