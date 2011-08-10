// Memory layout

#define PGSIZE          4096            // bytes mapped by a page
#define PGSHIFT         12              // log2(PGSIZE)

#define KSTKSIZE        (8*PGSIZE)              // size of a kernel stack

#define DEVSPACE  0xFE000000  // other devices are in the top of the phys address space
#define PHYSTOP   0xE000000 // use phys mem up to here as free pool

// Key addresses for address space layout (see kmap in vm.c for the layout)
#define KERNBASE 0xF0000000  // First kernel virtual address
#define USERTOP  (KERNBASE-PGSIZE)  // Highest user virtual address
#define KERNLINK 0xF0100000   // Address where kernel is linked

#ifndef __ASSEMBLER__

static inline uint v2p(void *a) { return (uint) a  - KERNBASE; }
static inline void *p2v(uint a) { return (void *) a + KERNBASE; }

#endif

#define V2P(a) ((uint) a - KERNBASE)
#define P2V(a) ((void *) a + KERNBASE)

#define V2P_WO(x) ((x) - KERNBASE)    // same as V2P, but without casts
#define P2V_WO(x) ((x) + KERNBASE)    // same as V2P, but without casts
