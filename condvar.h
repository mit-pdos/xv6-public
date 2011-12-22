#include "queue.h"

struct condvar {
  struct spinlock lock;
  LIST_HEAD(waiters, proc) waiters;
};
