#include "types.h"
#include "mmu.h"

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
