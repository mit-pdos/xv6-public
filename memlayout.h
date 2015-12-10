// Memory layout

#define EXTMEM  0x100000            // Start of extended memory, 1 MB
#define PHYSTOP 0xE000000           // Top physical memory, 224 MB
#define DEVSPACE 0xFE000000         // Other devices are at high addresses

// Key addresses for address space layout (see kmap in vm.c for layout)
#define KERNBASE 0x80000000         // First kernel virtual address
#define KERNLINK (KERNBASE+EXTMEM)  // Address where kernel is linked

#ifndef __ASSEMBLER__

// Convert virtual and physical addresses (from kernel's perspective)
#define V2P(a) (((uint) (a)) - KERNBASE)
#define P2V(a) (((void *) (a)) + KERNBASE)

#else

// The same without casts (for use in assembly)
#define V2P_WO(x) ((x) - KERNBASE)
#define P2V_WO(x) ((x) + KERNBASE)

#endif
