#define GET_PID(x)    (((x) >> 1) & 0x7FF)
#define SWAP_PERM(x)  ((x) & 1)

#define PACK(vpn, pid, sw) ((PTE_ADDR(vpn)) | ((pid) << 1) | sw)

#define RAMMAP_PAGES ((PHYSTOP - EXTMEM) / PGSIZE)
#define RDX_2_PA(rdx)  (EXTMEM + (rdx << 12))
#define PA_2_RDX(pa)   ((PTE_ADDR(pa) - EXTMEM) >> 12)
