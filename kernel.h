#include "types.h"

#define KBASE 0xFFFFFFFF80000000ull
#define KADDR(x) ((void*)(KBASE+(uintptr)(x)))
#define PADDR(x) ((uintptr)(x) - KBASE)
#define PGSIZE (2*1024*1024ull)

#define KCSEG (2<<3)  /* kernel code segment */
#define KDSEG (3<<3)  /* kernel data segment */

#define nil ((void*)0)

void	memmove(void *dst, void *src, int64 n);
int strlen(const char*);
