#include "vmx.h"
#include "amd64.h"

extern Segdesc bootgdt[NUMSEG];

void
inittrap(void)
{
  
}

// Bootstrap GDT.  Used by boot.S but defined in C
// so we can use the data structure macros in amd64.h.
Segdesc bootgdt[NUMSEG] = {
  SEGDESC(0, 0, 0),  // null
  SEGDESC(0, 0xfffff, SEG_R|SEG_CODE|SEG_S|SEG_DPL(0)|SEG_P|SEG_D|SEG_G),  // 32-bit kernel code
  SEGDESC(0, 0, SEG_R|SEG_CODE|SEG_S|SEG_DPL(0)|SEG_P|SEG_L|SEG_G),  // 64-bit kernel code
  SEGDESC(0, 0xfffff, SEG_W|SEG_S|SEG_DPL(0)|SEG_P|SEG_D|SEG_G),       // kernel data
};


#if 0

Intdesc idt[256];
Segdesc gdt[NUMSEG];

extern Segdesc bootgdt[NUMSEG];

extern uint64 trapentry[];
static Taskseg ts;
static char istack[8192];

void
inittrap(void)
{
	int i;
	uint32 bits;
	uint64 base, entry;
	
	bits = INT_P | SEG_INTR64;  // present, interrupt gate
	for(i=0; i<256; i++) {
		entry = trapentry[i];
		idt[i] = INTDESC(KCSEG, entry, bits);
	}

	memmove(gdt, bootgdt, sizeof gdt);
	base = (uintptr)&ts;
	gdt[TSSSEG>>3] = SEGDESC(base, (sizeof ts-1), SEG_P|SEG_TSS64A);
	gdt[(TSSSEG>>3)+1] = SEGDESCHI(base);
	
	ts.rsp0 = (uintptr)istack+sizeof istack;

	lidt(idt, sizeof idt);
	lgdt(gdt, sizeof gdt);
	ltr(TSSSEG);
}

void
trap(void)
{
	panic("trap");
}
#endif
