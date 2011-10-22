// This file defines amd64 data structures and constants.
// The constants are used by assembly too, so the data structures
// are guarded with #ifndef __ASSEMBLER__.

// CGATEXTMEM is the physical address of the CGA text memory.
#define CGATEXTMEM 0xB8000

// Segment descriptors.  See section 3.4.5 of 253668.pdf.

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

// Segment descriptor memory layout.
#ifndef __ASSEMBLER__
typedef struct Segdesc Segdesc;
struct Segdesc {
	uint16 limit0;
	uint16 base0;
	uint8 base1;
	uint8 bits;
	uint8 bitslimit1;
	uint8 base2;
};

// SEGDESC constructs a segment descriptor literal
// with the given, base, limit, and type bits.
#define SEGDESC(base, limit, bits) (Segdesc){ \
	(limit)&0xffff, (base)&0xffff, \
	((base)>>16)&0xff, \
	(bits)&0xff, \
	(((bits)>>4)&0xf0) | ((limit>>16)&0xf), \
	((base)>>24)&0xff, \
}

// SEGDESCHI constructs an extension segment descriptor
// literal that records the high bits of base.
#define SEGDESCHI(base) (Segdesc){ \
	((base)>>32)&0xffff, ((base)>>48)&0xffff, \
}
#endif

// Segment selectors (indexes) in our GDTs.
// Defined by our convention, not the architecture.
#define KCSEG32 (1<<3)  /* kernel 32-bit code segment */
#define KCSEG   (2<<3)  /* kernel code segment */
#define KDSEG   (3<<3)  /* kernel data segment */
#define TSSSEG  (4<<3)  /* tss segment - takes two slots */
#define UCSEG   (6<<3)  /* user code segment */
#define UDSEG   (7<<3)  /* user data segment */
#define NUMSEG  8

// Interrupt descriptors.
// See section 6.11 of 253668.pdf.

#define INT_DPL(x) ((x)<<5)    /* descriptor privilege level (2 bits) */
#define INT_P      (1<<7)      /* interrupt descriptor present */

#ifndef __ASSEMBLER__
typedef struct Intdesc Intdesc;
struct Intdesc
{
	uint16 rip0;
	uint16 cs;
	uint8 reserved0;
	uint8 bits;
	uint16 rip1;
	uint32 rip2;
	uint32 reserved1;
};

// INTDESC constructs an interrupt descriptor literal
// that records the given code segment, instruction pointer,
// and type bits.
#define INTDESC(cs, rip, bits) (Intdesc){ \
	(rip)&0xffff, (cs), 0, bits, ((rip)>>16)&0xffff, \
	(uint64)(rip)>>32, 0, \
}
#endif

// Task segment.
// See section 7.7 of 253668.pdf.
#ifndef __ASSEMBLER__
typedef struct Taskseg Taskseg;
struct Taskseg
{
	// I have no idea why they insisted on
	// misaligning all the uint64 fields.
	uint32	reserved0;
	uint64	rsp0;          /* stack pointers for priv levels 0, 1, 2 */
	uint64	rsp1;
	uint64	rsp2;
	uint32	reserved1;
	uint32	reserved2;
	uint64	ist1;          /* interrupt stack table pointers */
	uint64	ist2;
	uint64	ist3;
	uint64	ist4;
	uint64	ist5;
	uint64	ist6;
	uint64	ist7;
	uint32	reserved3;
	uint32	reserved4;
	uint16	reserved5;
	uint16	ioperm;        /* offset from TSS base to I/O permission bitmap */
} __attribute__((packed));
#endif

// Eflags register
#define FL_CF           0x00000001      // Carry Flag
#define FL_PF           0x00000004      // Parity Flag
#define FL_AF           0x00000010      // Auxiliary carry Flag
#define FL_ZF           0x00000040      // Zero Flag
#define FL_SF           0x00000080      // Sign Flag
#define FL_TF           0x00000100      // Trap Flag
#define FL_IF           0x00000200      // Interrupt Enable
#define FL_DF           0x00000400      // Direction Flag
#define FL_OF           0x00000800      // Overflow Flag
#define FL_IOPL_MASK    0x00003000      // I/O Privilege Level bitmask
#define FL_IOPL_0       0x00000000      //   IOPL == 0
#define FL_IOPL_1       0x00001000      //   IOPL == 1
#define FL_IOPL_2       0x00002000      //   IOPL == 2
#define FL_IOPL_3       0x00003000      //   IOPL == 3
#define FL_NT           0x00004000      // Nested Task
#define FL_RF           0x00010000      // Resume Flag
#define FL_VM           0x00020000      // Virtual 8086 mode
#define FL_AC           0x00040000      // Alignment Check
#define FL_VIF          0x00080000      // Virtual Interrupt Flag
#define FL_VIP          0x00100000      // Virtual Interrupt Pending
#define FL_ID           0x00200000      // ID flag
