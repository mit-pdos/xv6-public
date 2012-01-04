#ifndef NULL
#define NULL ((void *)0)
#endif

#define MIN(_a, _b)             \
  ({                            \
    __typeof__(_a) __a = (_a);  \
    __typeof__(_b) __b = (_b);  \
    __a <= __b ? __a : __b;     \
  })

#define MAX(_a, _b)             \
  ({                            \
    __typeof__(_a) __a = (_a);  \
    __typeof__(_b) __b = (_b);  \
    __a >= __b ? __a : __b;     \
  })

#define NELEM(x) (sizeof(x)/sizeof((x)[0]))

#define cmpswap(ptr, old, new) __sync_bool_compare_and_swap(ptr, old, new)
#define subfetch(ptr, val)     __sync_sub_and_fetch(ptr, val)
#define fetchadd(ptr, val)     __sync_fetch_and_add(ptr, val)
#define __offsetof             offsetof
