#include "types.h"
#include "mmu.h"
#include "spinlock.h"
#include "syscall.h"
#include "kern_c.h"

/*
 * Data structures that use C99 designated initializers,
 * which aren't avialable in C++11.
 */

struct segdesc  __attribute__((aligned(16))) bootgdt[NSEGS] = {
  // null
  [0]=SEGDESC(0, 0, 0),
  // 32-bit kernel code
  [1]=SEGDESC(0, 0xfffff, SEG_R|SEG_CODE|SEG_S|SEG_DPL(0)|SEG_P|SEG_D|SEG_G),
  // 64-bit kernel code
  [2]=SEGDESC(0, 0, SEG_R|SEG_CODE|SEG_S|SEG_DPL(0)|SEG_P|SEG_L|SEG_G),
  // kernel data
  [3]=SEGDESC(0, 0xfffff, SEG_W|SEG_S|SEG_DPL(0)|SEG_P|SEG_D|SEG_G),
  // The order of the user data and user code segments is
  // important for syscall instructions.  See initseg.
  // 64-bit user data
  [6]=SEGDESC(0, 0xfffff, SEG_W|SEG_S|SEG_DPL(3)|SEG_P|SEG_D|SEG_G),
  // 64-bit user code
  [7]=SEGDESC(0, 0, SEG_R|SEG_CODE|SEG_S|SEG_DPL(3)|SEG_P|SEG_L|SEG_G),
};

#define SYSCALL(name) [SYS_##name] = (void*)sys_##name

long (*syscalls[])(u64, u64, u64, u64, u64) = {
  SYSCALL(chdir),
  SYSCALL(close),
  SYSCALL(dup),
  SYSCALL(exec),
  SYSCALL(exit),
  SYSCALL(fork),
  SYSCALL(fstat),
  SYSCALL(getpid),
  SYSCALL(kill),
  SYSCALL(link),
  SYSCALL(mkdir),
  SYSCALL(mknod),
  SYSCALL(open),
  SYSCALL(pipe),
  SYSCALL(read),
  SYSCALL(sbrk),
  SYSCALL(sleep),
  SYSCALL(unlink),
  SYSCALL(wait),
  SYSCALL(write),
  SYSCALL(uptime),
  SYSCALL(map),
  SYSCALL(unmap),
  SYSCALL(halt),
  SYSCALL(socket),
  SYSCALL(bind),
  SYSCALL(listen),
  SYSCALL(accept),
  SYSCALL(pread),
  SYSCALL(async),
  SYSCALL(script),
};

