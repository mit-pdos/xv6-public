#include "param.h"
#if MTRACE
typedef __signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

typedef __PTRDIFF_TYPE__ intptr_t;
typedef unsigned __PTRDIFF_TYPE__ uintptr_t;

void* memcpy(void *dst, const void *src, u32 n);
char* strncpy(char *s, const char *t, int n);

#define RET_IP() ((unsigned long)__builtin_return_address(0))

#include <mtrace-magic.h>

// Tell mtrace about memory allocation
#define mtlabel(type, addr, bytes, str, n) \
  mtrace_label_register(type, addr, bytes, str, n, RET_IP())
#define mtunlabel(type, addr) \
  mtrace_label_register(type, addr, 0, NULL, 0, RET_IP())

// Tell mtrace about locking
#define mtlock(ptr) \
  mtrace_lock_register(RET_IP(), ptr, lockname(ptr), mtrace_lockop_acquire, 0)
#define mtacquired(ptr) \
  mtrace_lock_register(RET_IP(), ptr, lockname(ptr), mtrace_lockop_acquired, 0)  
#define mtunlock(ptr) \
  mtrace_lock_register(RET_IP(), ptr, lockname(ptr), mtrace_lockop_release, 0)

// Enable/disable all mtrace logging
#define mtenable(name)  mtrace_enable_set(1, name)
#define mtdisable(name) mtrace_enable_set(0, name)

// Log the number of operations 
static inline void mtops(u64 n)
{
  struct mtrace_appdata_entry entry;
  entry.u64 = 0;
  mtrace_appdata_register(&entry);
}

#include "mtrace-magic.h"
#else
#define mtlabel(type, addr, bytes, str, n) do { } while (0)
#define mtunlabel(type, addr) do { } while (0)
#define mtlock(ptr) do { } while (0)
#define mtacquired(ptr) do { } while (0)
#define mtunlock(ptr) do { } while (0)
#define mtrec(cpu) do { } while (0)
#define mtign(cpu) do { } while (0)
#define mtenable(name) do { } while (0)
#define mtdisable(name) do { } while (0)
#define mtops(n) do { } while (0)
#endif
