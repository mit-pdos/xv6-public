typedef unsigned char   u8;
typedef char            s8;
typedef unsigned short  u16;
typedef short           s16;
typedef unsigned int    u32;
typedef int             s32;
typedef unsigned long   u64;
typedef long            s64;
typedef u64             uptr;
typedef uptr            paddr;

// Page Map Entry (refers to any entry in any level)
typedef u64             pme_t;
typedef pme_t           pml4e_t;

#ifdef XV6
// POSIX types
typedef s64             ssize_t;
typedef u64             size_t;
typedef u64             off_t;
#endif
