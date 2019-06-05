#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

// Bootstrap processor starts running C code here.
// Allocate a real stack and switch to it, first
// doing some setup required for memory allocator to work.
void
main(int hartid)
{
  w_tp(hartid);    // save hartid where cpuid() can find it
  uartinit();      // serial port
  consoleinit();
  printf("entering main() on hart %d\n", hartid);
  kinit();         // physical page allocator
  kvminit();       // kernel page table
  procinit();      // process table
  trapinit();      // trap vectors
  plicinit();      // set up interrupt controller
  binit();         // buffer cache
  fileinit();      // file table
  ramdiskinit();   // disk
  userinit();      // first user process

  scheduler();        
}
