// Console input and output.
// Input is from the keyboard or serial port.
// Output is written to the screen and serial port.

#include "param.h"
#include "types.h"
#include "cpu.h"
#include "kernel.h"
#include "spinlock.h"

#include <stdarg.h>

static struct {
  struct spinlock lock;
  int locking;
} cons;

static void
printint(void (*putch) (void*, char), void *putarg,
         int xx, int base, int sign)
{
  static char digits[] = "0123456789abcdef";
  char buf[16];
  int i;
  u32 x;

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

// Print to the console.
static void
writecons(void *arg, char c)
{
  uartputc(c);
  cgaputc(c);
}

// Only understands %d, %x, %s.
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
      if(c == 'd'){
        printint(putch, putarg, va_arg(ap, u32), 10, 1);
      } else if(c == 'x') {
        printint(putch, putarg, va_arg(ap, u32), 16, 0);
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
    }
  }
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
  puts("panic: ");
  puts(s);
  puts("\n");

  for (;;);
}

void
initconsole(void)
{
  initlock(&cons.lock, "console");
  // XXX(sbw) enable once we setup %gs
  cons.locking = 0;
}
