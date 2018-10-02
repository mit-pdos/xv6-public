// This file contains definitions for the
// x86 memory management unit (MMU).

// Eflags register
#define FL_TF           0x00000100      // Trap Flag
#define FL_IF           0x00000200      // Interrupt Enable


// Control Register flags
#define CR0_PE          0x00000001      // Protection Enable
#define CR0_WP          0x00010000      // Write Protect
#define CR0_PG          0x80000000      // Paging

#define CR4_PSE         0x00000010      // Page size extension

// Segment selectors (indexes) in our GDTs.
// Defined by our convention, not the architecture.
#define KCSEG32 (1<<3)  /* kernel 32-bit code segment */
#define KCSEG   (2<<3)  /* kernel code segment */
#define KDSEG   (3<<3)  /* kernel data segment */
#define TSSSEG  (4<<3)  /* tss segment - takes two slots */
#define UDSEG   (6<<3)  /* user data segment */
#define UCSEG   (7<<3)  /* user code segment */

#define NSEGS 8

#ifndef __ASSEMBLER__
struct segdesc {
	uint16 limit0;
	uint16 base0;
	uint8 base1;
	uint8 bits;
	uint8 bitslimit1;
	uint8 base2;
};

// SEGDESC constructs a segment descriptor literal
// with the given, base, limit, and type bits.
#define SEGDESC(base, limit, bits) (struct segdesc){ \
	(limit)&0xffff, (base)&0xffff, \
	((base)>>16)&0xff, \
	(bits)&0xff, \
	(((bits)>>4)&0xf0) | ((limit>>16)&0xf), \
	((base)>>24)&0xff, \
}

// SEGDESCHI constructs an extension segment descriptor
// literal that records the high bits of base.
#define SEGDESCHI(base) (struct segdesc) {                        \
  (((base)>>32)&0xffff), (((base)>>48)&0xffff), \
}

#endif

#define DPL_USER    0x3     // User DPL

#define SEG_A      (1<<0)      /* segment accessed bit */
#define SEG_R      (1<<1)      /* readable (code) */
#define SEG_W      (1<<1)      /* writable (data) */
#define SEG_C      (1<<2)      /* conforming segment (code) */
#define SEG_E      (1<<2)      /* expand-down bit (data) */
#define SEG_CODE   (1<<3)      /* code segment (instead of data) */

// User and system segment bits.
#define SEG_S      (1<<4)      /* if 0, system descriptor */
#define SEG_DPL(x) ((x)<<5)    /* descriptor privilege level (2 bits) */
#define SEG_P      (1<<7)      /* segment present */
#define SEG_AVL    (1<<8)      /* available for operating system use */
#define SEG_L      (1<<9)      /* long mode */
#define SEG_D      (1<<10)     /* default operation size 32-bit */
#define SEG_G      (1<<11)     /* granularity */

// Application segment type bits
#define STA_X       0x8     // Executable segment
#define STA_W       0x2     // Writeable (non-executable segments)
#define STA_R       0x2     // Readable (executable segments)

// System segment type bits
#define SEG_LDT    (2<<0)      /* local descriptor table */
#define SEG_TSS64A (9<<0)      /* available 64-bit TSS */
#define SEG_TSS64B (11<<0)     /* busy 64-bit TSS */
#define SEG_CALL64 (12<<0)     /* 64-bit call gate */
#define SEG_INTR64 (14<<0)     /* 64-bit interrupt gate */
#define SEG_TRAP64 (15<<0)     /* 64-bit trap gate */

// A virtual address 'la' has a six-part structure as follows:
//
// +--16--+---9---+------9-------+-----9----+----9-------+----12-------+
// | Sign | PML4  |Page Directory| Page Dir |Page Table  | Offset Page |
// |Extend| Index | Pointer Index|  Index   |  Index     | in Page     |
// +------+-------+--------------+----------+------------+-------------+
//       \-PMX(va)-/\-PDPX(va)--/ \-PDX(va)-/ \-PTX(va)-/

// Page directory and page table constants.
#define NPDENTRIES      512    // # directory entries per page directory
#define PGSIZE          4096    // bytes mapped by a page
#define PGSHIFT         12      // offset of PTX in a linear address
#define PDXSHIFT        21      // offset of PDX in a linear address

#define PXMASK          0x1FF
#define PXSHIFT(n)	(PGSHIFT+(9*(n)))
#define PDX(va)         (((uint64)(va) >> PDXSHIFT) & PXMASK)
#define PX(n, va)	((((uint64) (va)) >> PXSHIFT(n)) & PXMASK)
#define L_PML4           3
    
// construct virtual address from indexes and offset
#define PGADDR(d, t, o) ((uint64)((d) << PDXSHIFT | (t) << PGSHIFT | (o)))

#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE-1))

// Page table/directory entry flags.
#define PTE_P           0x001   // Present
#define PTE_W           0x002   // Writeable
#define PTE_U           0x004   // User
#define PTE_PS          0x080   // Page Size
#define PTE_PWT         0x008   // Write-Through
#define PTE_PCD         0x010   // Cache-Disable

// Address in page table or page directory entry
#define PTE_ADDR(pte)   ((uint64)(pte) & ~0xFFF)
#define PTE_FLAGS(pte)  ((uint64)(pte) &  0xFFF)

#ifndef __ASSEMBLER__

typedef uint64 pml4e_t;
typedef uint64 pdpe_t;
typedef uint64 pte_t;

struct taskstate {
  uint8 reserved0[4];
  uint64 rsp[3];
  uint64 ist[8];
  uint8 reserved1[10];
  uint16 iomba;
  uint8 iopb[0];
} __attribute__ ((packed));

#define INT_P      (1<<7)      /* interrupt descriptor present */

struct intgate
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
#define INTDESC(cs, rip, bits) (struct intgate){ \
	(rip)&0xffff, (cs), 0, bits, ((rip)>>16)&0xffff, \
	(uint64)(rip)>>32, 0, \
}

// See section 4.6 of amd64 vol2
struct desctr
{
  uint16 limit;
  uint64 base;
} __attribute__((packed, aligned(16)));   // important!

#endif
