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

#if 0
#define __offsetof             offsetof
#else
#define __offsetof(type, field) (((uptr) &((type*)0x1000000)->field)-0x1000000)
#endif

#define __mpalign__ __attribute__((aligned(CACHELINE)))
#define __padout__  char __padout[0] __attribute__((aligned(CACHELINE)))
#define __noret__   __attribute__((noreturn))
