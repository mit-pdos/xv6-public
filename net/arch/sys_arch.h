#ifndef LWIP_ARCH_SYS_ARCH_H
#define LWIP_ARCH_SYS_ARCH_H

#include "spinlock.h"
#include "condvar.h"

typedef struct proc* sys_thread_t;

typedef u64 sys_prot_t;

typedef struct sys_mbox {
#define MBOXSLOTS 32
  struct spinlock s;
  struct condvar c;
  volatile int invalid;
  volatile int head;
  volatile int tail;
  void *msg[MBOXSLOTS];
} sys_mbox_t;

typedef struct sys_sem {
  struct spinlock s;
  struct condvar c;
  volatile u8 count;
} sys_sem_t;

#define SYS_ARCH_NOWAIT  0xfffffffe

#endif
