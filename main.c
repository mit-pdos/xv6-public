#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

// Bootstrap processor starts running C code here.
// Allocate a real stack and switch to it, first
// doing some setup required for memory allocator to work.
void
main()
{
  uartinit();      // serial port
  consoleinit();
  printf("entering main()\n");
  kinit();         // physical page allocator
  kvminit();       // kernel page table
  procinit();      // process table
  trapinit();      // trap vectors
#if 0
  binit();         // buffer cache
  fileinit();      // file table
  ideinit();       // disk
#endif
  userinit();      // first user process

  scheduler();        
}
