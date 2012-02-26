#pragma once

#define PGSIZE          4096
#define PGSHIFT		12		// log2(PGSIZE)

#define PXSHIFT(n)	(PGSHIFT+(9*(n)))
#define PX(n, la)	((((uptr) (la)) >> PXSHIFT(n)) & 0x1FF)

// Page table/directory entry flags.
#define PTE_P		0x001	// Present
#define PTE_W		0x002	// Writeable
#define PTE_U		0x004	// User
#define PTE_PWT		0x008	// Write-Through
#define PTE_PCD		0x010	// Cache-Disable
#define PTE_A		0x020	// Accessed
#define PTE_D		0x040	// Dirty
#define PTE_PS		0x080	// Page Size
#define PTE_G           0x100   // Global
#define PTE_MBZ		0x180	// Bits must be zero
#define PTE_LOCK        0x200   // xv6: lock
#define PTE_UNUSED      0x400   // xv6: unused
#define PTE_COW         0x800   // xv6: copy-on-write
#define PTE_NX		0x8000000000000000ull // No-execute enable

#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) ((__typeof__(a))((((uptr)(a)) & ~(PGSIZE-1))))

// Address in page table or page directory entry
#define PTE_ADDR(pte)	((uptr)(pte) & ~0xFFF)

#ifndef __ASSEMBLER__
struct segdesc {
  u16 limit0;
  u16 base0;
  u8 base1;
  u8 bits;
  u8 bitslimit1;
  u8 base2;
};
#endif

// SEGDESC constructs a segment descriptor literal
// with the given, base, limit, and type bits.
#define SEGDESC(base, limit, bits) { \
  (limit)&0xffff, (u16) ((base)&0xffff), \
  (u8) (((base)>>16)&0xff), \
  (bits)&0xff, \
  (((bits)>>4)&0xf0) | ((limit>>16)&0xf), \
  (u8) (((base)>>24)&0xff), \
}

// SEGDESCHI constructs an extension segment descriptor
// literal that records the high bits of base.
#define SEGDESCHI(base) { \
  (u16) (((base)>>32)&0xffff), (u16) (((base)>>48)&0xffff), \
}

// Segment selectors (indexes) in our GDTs.
// Defined by our convention, not the architecture.
#define KCSEG32 (1<<3)  /* kernel 32-bit code segment */
#define KCSEG   (2<<3)  /* kernel code segment */
#define KDSEG   (3<<3)  /* kernel data segment */
#define TSSSEG  (4<<3)  /* tss segment - takes two slots */
#define UDSEG   (6<<3)  /* user data segment */
#define UCSEG   (7<<3)  /* user code segment */
#define NSEGS   8

// User segment bits (SEG_S set).
#define SEG_A      (1<<0)      /* segment accessed bit */
#define SEG_R      (1<<1)      /* readable (code) */
#define SEG_W      (1<<1)      /* writable (data) */
#define SEG_C      (1<<2)      /* conforming segment (code) */
#define SEG_E      (1<<2)      /* expand-down bit (data) */
#define SEG_CODE   (1<<3)      /* code segment (instead of data) */

// System segment bits (SEG_S is clear).
#define SEG_LDT    (2<<0)      /* local descriptor table */
#define SEG_TSS64A (9<<0)      /* available 64-bit TSS */
#define SEG_TSS64B (11<<0)     /* busy 64-bit TSS */
#define SEG_CALL64 (12<<0)     /* 64-bit call gate */
#define SEG_INTR64 (14<<0)     /* 64-bit interrupt gate */
#define SEG_TRAP64 (15<<0)     /* 64-bit trap gate */

// User and system segment bits.
#define SEG_S      (1<<4)      /* if 0, system descriptor */
#define SEG_DPL(x) ((x)<<5)    /* descriptor privilege level (2 bits) */
#define SEG_P      (1<<7)      /* segment present */
#define SEG_AVL    (1<<8)      /* available for operating system use */
#define SEG_L      (1<<9)      /* long mode */
#define SEG_D      (1<<10)     /* default operation size 32-bit */
#define SEG_G      (1<<11)     /* granularity */

#ifndef __ASSEMBLER__
struct intdesc
{
  u16 rip0;
  u16 cs;
  u8 ist;
  u8 bits;
  u16 rip1;
  u32 rip2;
  u32 reserved1;
} __attribute__((packed, aligned(16)));

// See section 4.6 of amd64 vol2
struct desctr
{
  u16 limit;
  u64 base;
} __attribute__((packed, aligned(16)));

struct taskstate
{
  u8 reserved0[4];
  u64 rsp[3];
  u64 ist[8];
  u8 reserved1[10];
  u16 iomba;
  u8 iopb[0];
} __attribute__ ((packed, aligned(16)));
#endif

#define INT_P      (1<<7)      /* interrupt descriptor present */

// INTDESC constructs an interrupt descriptor literal
// that records the given code segment, instruction pointer,
// and type bits.
#define INTDESC(cs, rip, bits) (struct intdesc){ \
	(u16) ((rip)&0xffff), (cs), 0, bits, (u16) (((rip)>>16)&0xffff), \
	(u32) ((u64)(rip)>>32), 0, \
}
