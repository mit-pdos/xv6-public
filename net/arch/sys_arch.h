#ifndef LWIP_ARCH_SYS_ARCH_H
#define LWIP_ARCH_SYS_ARCH_H

#include "spinlock.h"
#include "condvar.h"

typedef struct proc* sys_thread_t;

typedef u64 sys_prot_t;

typedef struct sys_mbox {
#define MBOXSLOTS 32
  struct condvar c;
  volatile int invalid;
  volatile int head;
  volatile int tail;
  void *msg[MBOXSLOTS];
} sys_mbox_t;

typedef struct sys_sem {
  struct condvar c;
  volatile int invalid;
  volatile u8 count;
} sys_sem_t;

#define SYS_ARCH_NOWAIT  0xfffffffe

extern void lwip_core_sleep(struct condvar *);
extern void lwip_core_unlock(void);
extern void lwip_core_lock(void);
extern void lwip_core_init(void);

#endif
