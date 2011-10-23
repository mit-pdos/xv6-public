typedef unsigned short uint16;
typedef unsigned int uint32;
typedef long int64;
typedef unsigned long uint64;
typedef unsigned char uint8;
typedef uint64 uintptr;

typedef uint8 u8;
typedef uint16 u16;
typedef uint32 u32;
typedef int64 s64;
typedef uint64 u64;
typedef uint64 uptr;
typedef uptr paddr;

typedef u64 pml4e_t;
typedef u64 pdpe_t;
typedef u64 pde_t;
typedef u64 pte_t;

#define __mpalign__ __attribute__((aligned(CACHELINE)))
