#include "lwip/sys.h"

#define DIE panic(__func__)

//
// mbox
//
err_t
sys_mbox_new(sys_mbox_t *mbox, int size)
{
  DIE;
}

void
sys_mbox_set_invalid(sys_mbox_t *mbox)
{
  DIE;
}

err_t
sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
  DIE;
}

int
sys_mbox_valid(sys_mbox_t *mbox)
{
  DIE;
}

void
sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
  DIE;
}

void
sys_mbox_free(sys_mbox_t *mbox)
{
  DIE;
}

u32_t
sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
  DIE;
}

u32_t
sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
  DIE;
}

//
// sem
//
err_t
sys_sem_new(sys_sem_t *sem, u8_t count)
{
  DIE;
}

void
sys_sem_free(sys_sem_t *sem)
{
  DIE;
}

void
sys_sem_set_invalid(sys_sem_t *sem)
{
  DIE;
}

int
sys_sem_valid(sys_sem_t *sem)
{
  DIE;
}

void
sys_sem_signal(sys_sem_t *sem)
{
  DIE;
}

u32_t
sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  DIE;
}

//
// mutex
//
err_t
sys_mutex_new(sys_mutex_t *mutex)
{
  DIE;
}

void
sys_mutex_lock(sys_mutex_t *mutex)
{
  DIE;
}

void
sys_mutex_unlock(sys_mutex_t *mutex)
{
  DIE;
}

void
sys_mutex_free(sys_mutex_t *mutex)
{
  DIE;
}

//
// thread
//
sys_thread_t
sys_thread_new(const char *name, lwip_thread_fn thread, void *arg,
               int stacksize, int prio)
{
  DIE;
}

//
// init
//
void
sys_init(void)
{

}

