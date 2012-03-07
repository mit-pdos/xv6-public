#ifndef LWIP_ARCH_CC_H
#define LWIP_ARCH_CC_H

#include "types.h"

void lwip_cprintf(const char*, ...) __attribute__((format(printf, 1, 2)));
void lwip_panic(const char*, ...) __noret__ __attribute__((format(printf, 1, 2)));

typedef u32 u32_t;
typedef s32 s32_t;

typedef u64 u64_t;
typedef s64 s64_t;

typedef u16 u16_t;
typedef s16 s16_t;

typedef u8 u8_t;
typedef s8 s8_t;

typedef uptr mem_ptr_t;

#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT   __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

#define S16_F	"d"
#define U16_F	"u"
#define X16_F	"x"

#define S32_F	"d"
#define U32_F	"u"
#define X32_F	"x"

#define LWIP_PLATFORM_DIAG(x)	lwip_cprintf x
#define LWIP_PLATFORM_ASSERT(x)	lwip_panic(x)

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

// We define our own htonx so we can use them in userspace without
// compiling any lwIP .cc files (otherwise we must compile core/def.c).
#define LWIP_PLATFORM_BYTESWAP 1

static inline u16_t
LWIP_PLATFORM_HTONS(u16_t n)
{
  return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
}

static inline u32_t
LWIP_PLATFORM_HTONL(u32_t n)
{
  return ((n & 0xff) << 24) |
    ((n & 0xff00) << 8) |
    ((n & 0xff0000UL) >> 8) |
    ((n & 0xff000000UL) >> 24);
}

#endif
