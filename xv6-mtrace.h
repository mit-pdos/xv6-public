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

void* memcpy(void *dst, const void *src, uint n);
char* strncpy(char *s, const char *t, int n);

#define RET_EIP() ((unsigned long)__builtin_return_address(0))

#include "mtrace-magic.h"
