#ifndef LWIP_ARCH_CC_H
#define LWIP_ARCH_CC_H

#include <types.h>

void            panic(const char*) __attribute__((noreturn));
void            cprintf(const char*, ...);

typedef u32 u32_t;
typedef s32 s32_t;

typedef u64 u64_t;
typedef s64 s64_t;

typedef u16 u16_t;
typedef s16 s16_t;

typedef u8 u8_t;
typedef s8 s8_t;

typedef uptr mem_ptr_t;

#define PACK_STRUCT_FIELD(x)	x __attribute__((packed))
#define PACK_STRUCT_STRUCT  __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

#define S16_F	"d"
#define U16_F	"u"
#define X16_F	"x"

#define S32_F	"d"
#define U32_F	"u"
#define X32_F	"x"

#define LWIP_PLATFORM_DIAG(x)	cprintf x
#define LWIP_PLATFORM_ASSERT(x)	panic(x)

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

#endif
