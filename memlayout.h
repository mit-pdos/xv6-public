// Memory layout

#define EXTMEM  0x100000            // Start of extended memory
#define PHYSTOP 0xE000000           // Top physical memory
#define DEVSPACE 0xFE000000         // Other devices are at high addresses

// Key addresses for address space layout (see kmap in vm.c for layout)
#define KERNBASE 0xFFFF800000000000 // First kernel virtual address
#define DEVBASE  0xFFFF400000000000 // First device virtual address

#define KERNLINK (KERNBASE+EXTMEM)  // Address where kernel is linked

#ifndef __ASSEMBLER__

static inline addr_t v2p(void *a) {
  return ((addr_t) (a)) - ((addr_t)KERNBASE);
}
static inline void *p2v(addr_t a) {
  return (void *) ((a) + ((addr_t)KERNBASE));
}

#endif


#define V2P(a) (((addr_t) (a)) - KERNBASE)
#define P2V(a) (((void *) (a)) + KERNBASE)
#define IO2V(a) (((void *) (a)) + DEVBASE - DEVSPACE)

#define V2P_WO(x) ((x) - KERNBASE)    // same as V2P, but without casts
#define P2V_WO(x) ((x) + KERNBASE)    // same as P2V, but without casts
