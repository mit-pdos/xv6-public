// User/kernel shared mmap definitions
#pragma once

#define PROT_NONE  0x0
#define PROT_READ  0x1
#define PROT_WRITE 0x2
#define PROT_EXEC  0x4

#define MAP_SHARED    0x1
#define MAP_PRIVATE   0x2
#define MAP_FIXED     0x4
#define MAP_ANONYMOUS 0x8

#define MAP_FAILED ((void*)-1)
