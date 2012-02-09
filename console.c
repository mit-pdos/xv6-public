// Console input and output.
// Input is from the keyboard or serial port.
// Output is written to the screen and serial port.

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
#include "lib.h"
#include <stdarg.h>
#include "fmt.h"
#include <stddef.h>

#define BACKSPACE 0x100

static int panicked = 0;

static struct {
  struct spinlock lock;
  int locking;
} cons;

static void
consputc(int c)
{
  if(panicked){
    cli();
    for(;;)
      ;
  }

  switch(c) {
  case BACKSPACE:
    uartputc('\b');
    uartputc(' ');
    uartputc('\b');
    break;
  case '\n':
    uartputc('\r');
    // fall through
  default:
    uartputc(c);    
  }

  cgaputc(c);
}

// Print to the console.
static void
writecons(int c, void *arg)
{
  consputc(c);
}


// Print to a buffer.
struct bufstate {
  char *p;
  char *e;
};

static void
writebuf(int c, void *arg)
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

static void
__cprintf(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintfmt(writecons, 0, fmt, ap);
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
  u8 *p, *ep;

  p = (u8*)s;
  ep = p+strlen(s);

  for (; p < ep; p++)
    writecons(*p, NULL);

}

static inline void
printtrace(u64 rbp)
{
  uptr pc[10];

  getcallerpcs((void*)rbp, pc, NELEM(pc));
  for (int i = 0; i < NELEM(pc) && pc[i] != 0; i++)
    __cprintf("  %p\n", pc[i]);
}

void __noret__
kerneltrap(struct trapframe *tf)
{
  extern void sys_halt();
  const char *name = "(no name)";
  void *kstack = NULL;
  int pid = 0;

  cli();
  acquire(&cons.lock);

  if (myproc() != NULL) {
    if (myproc()->name && myproc()->name[0] != 0)
      name = myproc()->name;
    pid = myproc()->pid;
    kstack = myproc()->kstack;
  }
  __cprintf("kernel trap %u cpu %u\n"
          "  tf: rip %p rsp %p rbp %p cr2 %p cs %p\n"
          "  proc: name %s pid %u kstack %p\n",
          tf->trapno, mycpu()->id, 
          tf->rip, tf->rsp, tf->rbp, rcr2(), tf->cs,
          name, pid, kstack);
  printtrace(tf->rbp);

  panicked = 1;
  sys_halt();
  for(;;)
    ;
}

void
panic(const char *fmt, ...)
{
  extern void sys_halt();
  va_list ap;

  cli();
  acquire(&cons.lock);

  __cprintf("cpu%d: panic: ", mycpu()->id);
  va_start(ap, fmt);
  vprintfmt(writecons, 0, fmt, ap);
  va_end(ap);
  __cprintf("\n");
  printtrace(rrbp());

  panicked = 1;
  sys_halt();
  for(;;)
    ;
}

static int
consolewrite(struct inode *ip, char *buf, u32 off, u32 n)
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
    case C('E'):  // Print user-space PCs.
      for (u32 i = 0; i < NCPU; i++)
        cpus[i].timer_printpc = 1;
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
    case C('L'):  // Prof stats
      profdump();
      break;
    case C('K'):  // Prof enable
      profreset();
      cprintf("prof enabled\n");
      profenable = 1;
      break;
    case C('I'):  // Prof disable
      profenable = 0;
      cprintf("prof disabled\n");
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
consoleread(struct inode *ip, char *dst, u32 off, u32 n)
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
  initlock(&cons.lock, "console", LOCKSTAT_CONSOLE);
  cons.locking = 1;

  devsw[CONSOLE].write = consolewrite;
  devsw[CONSOLE].read = consoleread;

  picenable(IRQ_KBD);
  ioapicenable(IRQ_KBD, 0);
}
