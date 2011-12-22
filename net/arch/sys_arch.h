#ifndef LWIP_ARCH_SYS_ARCH_H
#define LWIP_ARCH_SYS_ARCH_H

#include "spinlock.h"
#include "condvar.h"

typedef int sys_thread_t;

typedef struct sys_mbox {
#define MBOXSLOTS 32
  void *msg[MBOXSLOTS];
} sys_mbox_t;

typedef struct sys_sem {
  struct spinlock s;
  struct condvar c;
  u8 count;
} sys_sem_t;

typedef struct sys_mutex {
  struct spinlock s;
} sys_mutex_t;

#define SYS_MBOX_NULL	(-1)
#define SYS_SEM_NULL	(-1)

void lwip_core_lock(void);
void lwip_core_unlock(void);
void lwip_core_init(void);

#define SYS_ARCH_DECL_PROTECT(lev)
#define SYS_ARCH_PROTECT(lev)
#define SYS_ARCH_UNPROTECT(lev)

#define SYS_ARCH_NOWAIT  0xfffffffe

#endif
