//
// Console input and output, to the uart.
// Implements erase/kill processing.
//

#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

#define BACKSPACE 0x100

void
consputc(int c)
{
  extern volatile int panicked; // from printf.c

  if(panicked){
    for(;;)
      ;
  }

  if(c == BACKSPACE){
    uartputc('\b'); uartputc(' '); uartputc('\b');
  } else
    uartputc(c);
}

struct {
  struct spinlock lock;
  
  // input
#define INPUT_BUF 128
  char buf[INPUT_BUF];
  uint r;  // Read index
  uint w;  // Write index
  uint e;  // Edit index
} cons;

#define C(x)  ((x)-'@')  // Contro

int
consoleread(int user_dst, uint64 dst, int n)
{
  uint target;
  int c;
  char buf[1];

  target = n;
  acquire(&cons.lock);
  while(n > 0){
    while(cons.r == cons.w){
      if(myproc()->killed){
        release(&cons.lock);
        return -1;
      }
      sleep(&cons.r, &cons.lock);
    }
    c = cons.buf[cons.r++ % INPUT_BUF];
    if(c == C('D')){  // EOF
      if(n < target){
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      break;
    }
    buf[0] = c;
    if(either_copyout(user_dst, dst, &buf[0], 1) == -1)
      break;
    dst++;
    --n;
    if(c == '\n')
      break;
  }
  release(&cons.lock);

  return target - n;
}

int
consolewrite(int user_src, uint64 src, int n)
{
  int i;

  acquire(&cons.lock);
  for(i = 0; i < n; i++){
    char c;
    if(either_copyin(&c, user_src, src+i, 1) == -1)
      break;
    consputc(c);
  }
  release(&cons.lock);

  return n;
}

void
consoleintr(int c)
{
  int doprocdump = 0;
  
  acquire(&cons.lock);

  switch(c){
  case C('P'):  // Process list.
    // procdump() locks cons.lock indirectly; invoke later
    doprocdump = 1;
    break;
  case C('U'):  // Kill line.
    while(cons.e != cons.w &&
          cons.buf[(cons.e-1) % INPUT_BUF] != '\n'){
      cons.e--;
      consputc(BACKSPACE);
    }
    break;
  case C('H'): case '\x7f':  // Backspace
    if(cons.e != cons.w){
      cons.e--;
      consputc(BACKSPACE);
    }
    break;
  default:
    if(c != 0 && cons.e-cons.r < INPUT_BUF){
      c = (c == '\r') ? '\n' : c;
      cons.buf[cons.e++ % INPUT_BUF] = c;
      consputc(c);
      if(c == '\n' || c == C('D') || cons.e == cons.r+INPUT_BUF){
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    break;
  }
  
  release(&cons.lock);

  if(doprocdump)
    procdump();
}

void
consoleinit(void)
{
  initlock(&cons.lock, "cons");

  devsw[CONSOLE].write = consolewrite;
  devsw[CONSOLE].read = consoleread;
}
