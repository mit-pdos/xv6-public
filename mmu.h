// This file contains definitions for the
// x86 memory management unit (MMU).

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

// Control Register flags
#define CR0_PE          0x00000001      // Protection Enable
#define CR0_MP          0x00000002      // Monitor coProcessor
#define CR0_EM          0x00000004      // Emulation
#define CR0_TS          0x00000008      // Task Switched
#define CR0_ET          0x00000010      // Extension Type
#define CR0_NE          0x00000020      // Numeric Errror
#define CR0_WP          0x00010000      // Write Protect
#define CR0_AM          0x00040000      // Alignment Mask
#define CR0_NW          0x20000000      // Not Writethrough
#define CR0_CD          0x40000000      // Cache Disable
#define CR0_PG          0x80000000      // Paging

#define CR4_PSE         0x00000010      // Page size extension
#define CR4_PAE         0x00000020      // Physical address extensions
#define CR4_OSXFSR      0x00000200      // OS supports FXSAVE and FXRSTOR
#define CR4_OSXMMEXCPT  0x00000400      // OS supports SSE exceptions

// Model specific registers
#define MSR_EFER	0xC0000080	// extended feature enable register
#define MSR_STAR 	0xC0000081	// stores ring 0's and ring 3's segment bases
#define MSR_LSTAR	0xC0000082	// stores syscall's entry rip
#define MSR_CSTAR	0xC0000083	// for compatiblity mode (not used)
#define MSR_SFMASK	0xC0000084	// syscall flag mask

// The CS values for user and kernel space
#define USER_CS		35
#define KERNEL_CS	8
#define USER_DS		0X2B

// various segment selectors.
#define SEG_KCODE 1  // kernel code
#define SEG_KDATA 2  // kernel data+stack
#define SEG_KCPU  3  // kernel per-cpu data
#define SEG_UCODE 4  // user code
#define SEG_UDATA 5  // user data+stack
#define SEG_TSS   6  // this process's task state

// cpu->gdt[NSEGS] holds the above segments.
#define NSEGS     7
#define CALL_GATE 8

//PAGEBREAK!
#ifndef __ASSEMBLER__
// Segment Descriptor
struct segdesc {
  uint lim_15_0 : 16;  // Low bits of segment limit
  uint base_15_0 : 16; // Low bits of segment base address
  uint base_23_16 : 8; // Middle bits of segment base address
  uint type : 4;       // Segment type (see STS_ constants)
  uint s : 1;          // 0 = system, 1 = application
  uint dpl : 2;        // Descriptor Privilege Level
  uint p : 1;          // Present
  uint lim_19_16 : 4;  // High bits of segment limit
  uint avl : 1;        // Unused (available for software use)
  uint rsv1 : 1;       // 64-bit segment
  uint db : 1;         // 0 = 16-bit segment, 1 = 32-bit segment
  uint g : 1;          // Granularity: limit scaled by 4K when set
  uint base_31_24 : 8; // High bits of segment base address
};

// Normal segment
#define SEG(type, lim, base, sys, dpl, rsv) (struct segdesc)   \
{ (addr_t)(lim) & 0xffff, (uint)(base) & 0xffff,      \
  ((addr_t)(base) >> 16) & 0xff, type, sys, dpl, 1,       \
  (addr_t)(lim) >> 60, 0, rsv, 0, 1, (addr_t)(base) >> 24 }

#define SEG16(type, base, lim, dpl) (struct segdesc)  \
{ (lim) & 0xffff, (uint)(base) & 0xffff,              \
  ((addr_t)(base) >> 16) & 0xff, type, 1, dpl, 1,       \
  (addr_t)(lim) >> 16, 0, 0, 1, 0, (addr_t)(base) >> 24 }
#endif

#define DPL_USER    0x3     // User DPL
#define APP_SEG	    0x1

// Application segment type bits
#define STA_X       0x8     // Executable segment
#define STA_E       0x4     // Expand down (non-executable segments)
#define STA_C       0x4     // Conforming code segment (executable only)
#define STA_W       0x2     // Writeable (non-executable segments)
#define STA_R       0x2     // Readable (executable segments)
#define STA_A       0x1     // Accessed

// System segment type bits
#define STS_T16A    0x1     // Available 16-bit TSS
#define STS_LDT     0x2     // Local Descriptor Table
#define STS_T16B    0x3     // Busy 16-bit TSS
#define STS_CG16    0x4     // 16-bit Call Gate
#define STS_TG      0x5     // Task Gate / Coum Transmitions
#define STS_IG16    0x6     // 16-bit Interrupt Gate
#define STS_TG16    0x7     // 16-bit Trap Gate
#define STS_T64A    0x9     // Available 64-bit TSS
#define STS_T64B    0xB     // Busy 64-bit TSS
#define STS_CG64    0xC     // 64-bit Call Gate
#define STS_IG64    0xE     // 64-bit Interrupt Gate
#define STS_TG64    0xF     // 64-bit Trap Gate

// A virtual address 'la' has a six-part structure as follows:
//
// +--16--+---9---+------9-------+-----9----+----9-------+----12-------+
// | Sign | PML4  |Page Directory| Page Dir |Page Table  | Offset Page |
// |Extend| Index | Pointer Index|  Index   |  Index     | in Page     |
// +------+-------+--------------+----------+------------+-------------+
//       \-PMX(va)-/\-PDPX(va)--/ \-PDX(va)-/ \-PTX(va)-/

// page map level 4 index
#define PMX(va)         (((addr_t)(va) >> PML4XSHIFT) & PXMASK)
// page directory pointer index
#define PDPX(va)         (((addr_t)(va) >> PDPXSHIFT) & PXMASK)
// page directory index
#define PDX(va)         (((addr_t)(va) >> PDXSHIFT) & PXMASK)
// page table index
#define PTX(va)         (((addr_t)(va) >> PTXSHIFT) & PXMASK)

// Page directory and page table constants.
#define NPDENTRIES      512     // # directory entries per page directory
#define NPTENTRIES      512     // # PTEs per page table
#define PGSIZE          4096    // bytes mapped by a page
#define PGSHIFT         12      // log2(PGSIZE)
#define PTXSHIFT        12      // offset of PTX in a linear address
#define PDXSHIFT        21      // offset of PDX in a linear address
#define PDPXSHIFT       30      // offset of PDPX in a linear address
#define PML4XSHIFT      39      // offset of PML4X in a linear address
#define PXMASK          0X1FF

#define PGROUNDUP(sz)  (((sz)+((addr_t)PGSIZE-1)) & ~((addr_t)(PGSIZE-1)))
#define PGROUNDDOWN(a) (((a)) & ~((addr_t)(PGSIZE-1)))

// Page table/directory entry flags.
#define PTE_P           0x001   // Present
#define PTE_W           0x002   // Writeable
#define PTE_U           0x004   // User
#define PTE_PWT         0x008   // Write-Through
#define PTE_PCD         0x010   // Cache-Disable
#define PTE_A           0x020   // Accessed
#define PTE_D           0x040   // Dirty
#define PTE_PS          0x080   // Page Size
#define PTE_MBZ         0x180   // Bits must be zero

// Address in page table or page directory entry
#define PTE_ADDR(pte)   ((addr_t)(pte) & ~0xFFF)
#define PTE_FLAGS(pte)  ((addr_t)(pte) &  0xFFF)

#define TRAP_GATE	0x100	// trap gate if one, interrupt gate if zero

#ifndef __ASSEMBLER__
typedef addr_t pte_t;

// Task state segment format.
// This is only used to specify the new stack address after interrupt

struct taskstate {
  uint link;         // Old ts selector
  uint esp0;         // Stack pointers and segment selectors
  ushort ss0;        //   after an increase in privilege level
  ushort padding1;
  uint *esp1;
  ushort ss1;
  ushort padding2;
  uint *esp2;
  ushort ss2;
  ushort padding3;
  void *cr3;         // Page directory base
  uint *eip;         // Saved state from last task switch
  uint eflags;
  uint eax;          // More saved state (registers)
  uint ecx;
  uint edx;
  uint ebx;
  uint *esp;
  uint *ebp;
  uint esi;
  uint edi;
  ushort es;         // Even more saved state (segment selectors)
  ushort padding4;
  ushort cs;
  ushort padding5;
  ushort ss;
  ushort padding6;
  ushort ds;
  ushort padding7;
  ushort fs;
  ushort padding8;
  ushort gs;
  ushort padding9;
  ushort ldt;
  ushort padding10;
  ushort t;          // Trap on task switch
  ushort iomb;       // I/O map base address
};

// PAGEBREAK: 12
// Gate descriptors for interrupts and traps
struct gatedesc {
  uint off_15_0 : 16;   // low 16 bits of offset in segment
  uint cs : 16;         // code segment selector
  uint args : 5;        // # args, 0 for interrupt/trap gates
  uint rsv1 : 3;        // reserved(should be zero I guess)
  uint type : 4;        // type(STS_{TG,IG32,TG32})
  uint s : 1;           // must be 0 (system)
  uint dpl : 2;         // descriptor(meaning new) privilege level
  uint p : 1;           // Present
  uint off_31_16 : 16;  // high bits of offset in segment
  uint32 off_63_32;
  uint32 rsv2;
};

// Set up a normal interrupt/trap gate descriptor.
// - istrap: 1 for a trap (= exception) gate, 0 for an interrupt gate.
//   interrupt gate clears FL_IF, trap gate leaves FL_IF alone
// - sel: Code segment selector for interrupt/trap handler
// - off: Offset in code segment for interrupt/trap handler
// - dpl: Descriptor Privilege Level -
//        the privilege level required for software to invoke
//        this interrupt/trap gate explicitly using an int instruction.
#define SETCALLGATE(gate, cs, off, d)   \
{                                                         \
  (gate)->off_15_0 = (uint32)(off) & 0xffff;                \
  (gate)->cs = (cs);                                      \
  (gate)->args = 0;                                        \
  (gate)->rsv1 = 0;                                        \
  (gate)->type = STS_CG64;                                 \
  (gate)->s = 0;                                           \
  (gate)->dpl = (d);                                       \
  (gate)->p = 1;                                           \
  (gate)->off_31_16 = ((uint)(off) >> 16) & 0xffff;        \
  (gate)->off_63_32 = ((uint)(off) >> 32) & 0xffffffff;    \
  (gate)->rsv2 = 0;					  \
};
#endif
