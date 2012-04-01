#pragma once

#if defined (LINUX)
typedef pthread_spinlock_t wqlock_t;
#elif defined(XV6_KERNEL)
#include "types.h"
#include "spinlock.h"
typedef struct spinlock wqlock_t;
#else
#include "uspinlock.h"
typedef struct uspinlock wqlock_t;
#endif
