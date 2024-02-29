#include "spinlock.h"

struct condvar {
    struct spinlock lk;
};