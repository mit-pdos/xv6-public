#define PGSIZE          4096
#define PGSHIFT		12		// log2(PGSIZE)

#define PTXSHIFT	12		// offset of PTX in a linear address
#define PDXSHIFT	21		// offset of PDX in a linear address
#define PDPXSHIFT       30              // offset of PDPX in a linear address
#define PML4XSHIFT      39              // offset of PML4X in a linear address

// page table index
#define PTX(la)		(((uptr)(la) >> PTXSHIFT) & 0x1FF)

// page directory index
#define PDX(la)		(((uptr)(la) >> PDXSHIFT) & 0x1FF)

// page diretory pointer index
#define PDPX(la)       (((uptr)(la) >> PDPXSHIFT) & 0x1FF)

// page map level 4 index
#define PML4X(la)       (((uptr)(la) >> PML4XSHIFT) & 0x1FF)

// Page table/directory entry flags.
#define PTE_P		0x001	// Present
#define PTE_W		0x002	// Writeable
#define PTE_U		0x004	// User
#define PTE_PWT		0x008	// Write-Through
#define PTE_PCD		0x010	// Cache-Disable
#define PTE_A		0x020	// Accessed
#define PTE_D		0x040	// Dirty
#define PTE_PS		0x080	// Page Size
#define PTE_MBZ		0x180	// Bits must be zero
#define PTE_COW         0x800   // copy-on-write

#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) ((__typeof__(a))((((uptr)(a)) & ~(PGSIZE-1))))

// Address in page table or page directory entry
#define PTE_ADDR(pte)	((uptr)(pte) & ~0xFFF)
