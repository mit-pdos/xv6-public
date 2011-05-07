#include <stdint.h>

void* memcpy(void *dst, const void *src, uint n);
char* strncpy(char *s, const char *t, int n);

#define RET_EIP() ((unsigned long)__builtin_return_address(0))

#include "mtrace-magic.h"
