// Console input and output.
// Input is from the keyboard or serial port.
// Output is written to the screen and serial port.

#include "param.h"
#include "types.h"
#include "cpu.h"
#include "kernel.h"
#include "spinlock.h"
#include "fs.h"
#include "condvar.h"
#include "file.h"
#include "x86.h"

#include <stdarg.h>

#define BACKSPACE 0x100

static struct {
  struct spinlock lock;
  int locking;
} cons;

static void
printint(void (*putch) (void*, char), void *putarg,
         u64 xx, int base, int sign)
{
  static char digits[] = "0123456789abcdef";
  char buf[16];
  int i;
  u64 x;

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
    putch(putarg, buf[i]);
}

static void
consputc(int c)
{
  // XXX(sbw)
#if 0
  if(panicked){
    cli();
    for(;;)
      ;
  }
#endif

  if(c == BACKSPACE){
    uartputc('\b'); uartputc(' '); uartputc('\b');
  } else
    uartputc(c);
  cgaputc(c);
}

// Print to the console.
static void
writecons(void *arg, char c)
{
  consputc(c);
}

// Only understands %d, %u, %x, %s, %lx.
void
vprintfmt(void (*putch) (void*, char), void *putarg,
          const char *fmt, va_list ap)
{
  char *s;
  int c, i, state;

  state = 0;
  for(i = 0; fmt[i]; i++){
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
        state = '%';
      } else {
        putch(putarg, c);
      }
    } else if(state == '%'){
      if(c == 'd') {
        printint(putch, putarg, va_arg(ap, u32), 10, 1);
      } else if(c == 'u') {
        printint(putch, putarg, va_arg(ap, u32), 10, 0);
      } else if(c == 'x') {
        printint(putch, putarg, va_arg(ap, u32), 16, 0);
      } else if(c == 'l') {
        state = 'l';
        continue;
      } else if(c == 's'){
        s = (char*) va_arg(ap, char*);
        if(s == 0)
          s = "(null)";
        while(*s != 0){
          putch(putarg, *s);
          s++;
        }
      } else if(c == 'c'){
        putch(putarg, va_arg(ap, u32));
      } else if(c == '%'){
        putch(putarg, c);
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putch(putarg, '%');
        putch(putarg, c);
      }
      state = 0;
    } else if(state == 'l') {
      if(c == 'x') {
        printint(putch, putarg, va_arg(ap, u64), 16, 0);
      }
      else {
        // Unknown % sequence.  Print it to draw attention.
        putch(putarg, '%');
        putch(putarg, c);
      }
      state = 0;
    }
  }
}

// Print to a buffer.
struct bufstate {
  char *p;
  char *e;
};

static void
writebuf(void *arg, char c)
{
  struct bufstate *bs = arg;
  if (bs->p < bs->e) {
    bs->p[0] = c;
    bs->p++;
  }
}

void
vsnprintf(char *buf, u32 n, char *fmt, va_list ap)
{
  struct bufstate bs = { buf, buf+n-1 };
  vprintfmt(writebuf, (void*) &bs, fmt, ap);
  bs.p[0] = '\0';
}

void
snprintf(char *buf, u32 n, char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(buf, n, fmt, ap);
  va_end(ap);
}

void
cprintf(const char *fmt, ...)
{
  va_list ap;

  int locking = cons.locking;
  if(locking)
    acquire(&cons.lock);

  va_start(ap, fmt);
  vprintfmt(writecons, 0, fmt, ap);
  va_end(ap);

  if(locking)
    release(&cons.lock);
}

void
puts(const char *s)
{
  uint8 *p, *ep;

  p = (uint8*)s;
  ep = p+strlen(s);

  for (; p < ep; p++)
    writecons(NULL, *p);

}

void __attribute__((noreturn))
panic(const char *s)
{
  cli();
  cons.locking = 0;
  cprintf("cpu%d: panic: ", mycpu()->id);
  cprintf(s);
  cprintf("\n");

  extern void sys_halt();
  sys_halt();

  for(;;)
    ;
}

static int
consolewrite(struct inode *ip, char *buf, int n)
{
  int i;

  iunlock(ip);
  acquire(&cons.lock);
  for(i = 0; i < n; i++)
    consputc(buf[i] & 0xff);
  release(&cons.lock);
  ilock(ip, 1);

  return n;
}

static int
consoleread(struct inode *ip, char *dst, int n)
{
  panic("consoleread");
}

void
initconsole(void)
{
  initlock(&cons.lock, "console");
  devsw[CONSOLE].write = consolewrite;
  devsw[CONSOLE].read = consoleread;

  // XXX(sbw) enable once we setup %gs
  cons.locking = 0;
}
