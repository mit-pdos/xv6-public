#include "types.h"
#include "amd64.h"
#include "kernel.hh"
#include "mmu.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.hh"
#include "cpu.hh"
#include "vm.hh"
#include "sperf.hh"
#include "kmtrace.hh"
#include "futex.h"

int
sys_fork(int flags)
{
  ANON_REGION(__func__, &perfgroup);
  return fork(flags);
}

int
sys_exit(void)
{
  exit();
  panic("exit() returned");
}

int
sys_wait(void)
{
  ANON_REGION(__func__, &perfgroup);
  return wait();
}

int
sys_kill(int pid)
{
  return proc::kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

char*
sys_sbrk(int n)
{
  uptr addr;

  if(myproc()->vmap->sbrk(n, &addr) < 0)
    return (char*)-1;
  return (char*)addr;
}

int
sys_nsleep(u64 nsec)
{
  struct spinlock lock;
  struct condvar cv;
  u64 nsecto;

  initlock(&lock, "sleep_lock", 0);
  initcondvar(&cv, "sleep_cv");
  acquire(&lock);

  auto cleanup = scoped_cleanup([&lock, &cv]() { 
    release(&lock);
    destroycondvar(&cv);
    destroylock(&lock);
  });

  nsecto = nsectime()+nsec;
  while (nsecto > nsectime()) {
    if (myproc()->killed)
      return -1;
    cv_sleepto(&cv, &lock, nsecto);
  }
  return 0;
}

// return how many clock tick interrupts have occurred
// since boot.
u64
sys_uptime(void)
{
  return nsectime();
}

int
sys_map(userptr<void> addr, size_t len)
{
  ANON_REGION(__func__, &perfgroup);

#if MTRACE
  mt_ascope ascope("%s(%p,%lx)", __func__, addr, len);
  mtwriteavar("thread:%x", myproc()->pid);
  for (uptr i = PGROUNDDOWN(addr); i < PGROUNDUP(addr + len); i += PGSIZE)
    mtwriteavar("page:%016x", i);
#endif

  vmnode *vmn = new vmnode(PGROUNDUP(len) / PGSIZE);
  if (vmn == 0)
    return -1;

  long r = myproc()->vmap->insert(vmn, PGROUNDDOWN(addr), 1);
  if (r < 0) {
    delete vmn;
    return -1;
  }

  return r;
}

int
sys_unmap(userptr<void> addr, size_t len)
{
  ANON_REGION(__func__, &perfgroup);

#if MTRACE
  mt_ascope ascope("%s(%p,%lx)", __func__, addr, len);
  mtwriteavar("thread:%x", myproc()->pid);
  for (uptr i = PGROUNDDOWN(addr); i < PGROUNDUP(addr + len); i += PGSIZE)
    mtwriteavar("page:%016x", i);
#endif

  uptr align_addr = PGROUNDDOWN(addr);
  uptr align_len = PGROUNDUP(addr + len) - align_addr;
  if (myproc()->vmap->remove(align_addr, align_len) < 0)
    return -1;

  return 0;
}

int
sys_halt(void)
{
  int i;
  const char s[] = "Shutdown";

  for(i = 0; i < 8; i++)
    outb(0x8900, s[i]);
  return 0;
}

int
sys_setfs(u64 base)
{
  proc *p = myproc();
  p->user_fs_ = base;
  switchvm(p);
  return 0;
}

int
sys_setaffinity(int cpu)
{
  return myproc()->set_cpu_pin(cpu);
}

long
sys_futex(const u64* addr, int op, u64 val, u64 timer)
{
  futexkey_t key;

  if (futexkey(addr, myproc()->vmap, &key) < 0)
    return -1;

  switch(op) {
  case FUTEX_WAIT:
    return futexwait(key, val, timer);
  case FUTEX_WAKE:
    return futexwake(key, val);
  default:
    return -1;
  }
}
