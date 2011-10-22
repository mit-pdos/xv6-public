#include "vmx.h"
#include "amd64.h"

extern Segdesc bootgdt[NUMSEG];

// Bootstrap GDT.  Used by boot.S but defined in C
// so we can use the data structure macros in amd64.h.
Segdesc bootgdt[NUMSEG] = {
	SEGDESC(0, 0, 0),  // null
	SEGDESC(0, 0xfffff, SEG_R|SEG_CODE|SEG_S|SEG_DPL(0)|SEG_P|SEG_D|SEG_G),  // 32-bit kernel code
	SEGDESC(0, 0, SEG_R|SEG_CODE|SEG_S|SEG_DPL(0)|SEG_P|SEG_L|SEG_G),  // 64-bit kernel code
	SEGDESC(0, 0xfffff, SEG_W|SEG_S|SEG_DPL(0)|SEG_P|SEG_D|SEG_G),       // kernel data
};
