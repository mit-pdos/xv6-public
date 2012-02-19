#define __padout__  char __padout[0] __attribute__((aligned(CACHELINE)))
#define __mpalign__ __attribute__((aligned(CACHELINE)))
#define __noret__   __attribute__((noreturn))
#define barrier() __asm volatile("" ::: "memory")

#ifdef __cplusplus
#define BEGIN_DECLS extern "C" {
#define END_DECLS   }
#else
#define BEGIN_DECLS
#define END_DECLS
#endif
