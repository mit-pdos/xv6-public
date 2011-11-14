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
#include "amd64.h"
#include "queue.h"
#include "proc.h"
#include "traps.h"
#include <stdarg.h>

#define BACKSPACE 0x100

static int panicked = 0;

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

// Print to the console.
static void
writecons(void *arg, char c)
{
  consputc(c);
}

// Only understands %d, %u, %x, %s, %lx, %lu.
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
      else if(c == 'u') {
        printint(putch, putarg, va_arg(ap, u64), 10, 0);
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

static inline void
stacktrace(void)
{
#define PRINT_RET(i)                                       \
  do {                                                     \
    uptr addr = (uptr) __builtin_return_address(i);        \
    if ((addr & KBASE) == KBASE)                           \
      cprintf("  %lx\n", addr);                            \
    else                                                   \
      return;                                              \
} while (0)

  PRINT_RET(0);
  PRINT_RET(1);
  PRINT_RET(2);
  PRINT_RET(3);
  PRINT_RET(4);
  PRINT_RET(5);
  PRINT_RET(6);

#undef PRINT_RET
}

void __attribute__((noreturn))
panic(const char *s)
{
  extern void sys_halt();

  cli();
  
  if (cons.locking)
    acquire(&cons.lock);
  cons.locking = 0;

  cprintf("cpu%d: panic: ", mycpu()->id);
  cprintf(s);
  cprintf("\n");
  stacktrace();
  panicked = 1;
  // Never release cons.lock
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

#define INPUT_BUF 128
struct {
  struct spinlock lock;
  struct condvar cv;
  char buf[INPUT_BUF];
  int r;  // Read index
  int w;  // Write index
  int e;  // Edit index
} input;

#define C(x)  ((x)-'@')  // Control-x

void
consoleintr(int (*getc)(void))
{
  int c;

  acquire(&input.lock);
  while((c = getc()) >= 0){
    switch(c){
    case C('P'):  // Process listing.
      procdumpall();
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
    case C('W'):  // Work queue stats
      wq_dump();
      break;
    default:
      if(c != 0 && input.e-input.r < INPUT_BUF){
        c = (c == '\r') ? '\n' : c;
        input.buf[input.e++ % INPUT_BUF] = c;
        consputc(c);
        if(c == '\n' || c == C('D') || input.e == input.r+INPUT_BUF){
          input.w = input.e;
          cv_wakeup(&input.cv);
        }
      }
      break;
    }
  }
  release(&input.lock);
}

static int
consoleread(struct inode *ip, char *dst, int n)
{
  int target;
  int c;

  iunlock(ip);
  target = n;
  acquire(&input.lock);
  while(n > 0){
    while(input.r == input.w){
      if(myproc()->killed){
        release(&input.lock);
        ilock(ip, 1);
        return -1;
      }
      cv_sleep(&input.cv, &input.lock);
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
  ilock(ip, 1);

  return target - n;
}

void
initconsole(void)
{
  initlock(&cons.lock, "console");
  cons.locking = 1;

  devsw[CONSOLE].write = consolewrite;
  devsw[CONSOLE].read = consoleread;

  picenable(IRQ_KBD);
  ioapicenable(IRQ_KBD, 0);
}
