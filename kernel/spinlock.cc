// Mutual exclusion spin locks.

#include "types.h"
#include "kernel.hh"
#include "amd64.h"
#include "cpu.hh"
#include "bits.hh"
#include "spinlock.h"
#include "mtrace.h"
#include "condvar.h"
#include "fs.h"
#include "file.hh"

#if LOCKSTAT
static int lockstat_enable;

static inline struct cpulockstat *
mylockstat(struct spinlock *lk)
{
  return &lk->stat->s.cpu[cpunum()];
}

void*
klockstat::operator new(unsigned long nbytes)
{
  assert(nbytes == sizeof(klockstat));
  return kmalloc(sizeof(klockstat));
}

void
klockstat::operator delete(void *p)
{
  return kmfree(p, sizeof(klockstat));
}
#endif

static inline void
locking(struct spinlock *lk)
{
#if SPINLOCK_DEBUG
  if(holding(lk)) {
    cprintf("%p\n", __builtin_return_address(0));
    panic("acquire");
  }
#endif

#if LOCKSTAT
  if (lockstat_enable && lk->stat != NULL)
    mylockstat(lk)->locking_ts = rdtsc();
#endif

  mtlock(lk);
}

static inline void
locked(struct spinlock *lk, u64 retries)
{
  mtacquired(lk);
  
#if SPINLOCK_DEBUG
  // Record info about lock acquisition for debugging.
  lk->cpu = mycpu();
  getcallerpcs(&lk, lk->pcs, NELEM(lk->pcs));
#endif

#if LOCKSTAT
  if (lockstat_enable && lk->stat != NULL) {
    struct cpulockstat *s = mylockstat(lk);
    if (retries > 0)
      s->contends++;
    s->acquires++;
    s->locked_ts = rdtsc();
  }
#endif
}

static inline void
releasing(struct spinlock *lk)
{
#if SPINLOCK_DEBUG
  if(!holding(lk)) {
    cprintf("lock: %s\n", lk->name);
    panic("release");
  }
#endif

  mtunlock(lk);

#if SPINLOCK_DEBUG
  lk->pcs[0] = 0;
  lk->cpu = 0;
#endif

#if LOCKSTAT
  if (lockstat_enable && lk->stat != NULL) {
    struct cpulockstat *s = mylockstat(lk);
    u64 ts = rdtsc();
    s->locking += ts - s->locking_ts;
    s->locked += ts - s->locked_ts;
  }
#endif
}

// Check whether this cpu is holding the lock.
#if SPINLOCK_DEBUG
int
holding(struct spinlock *lock)
{
  return lock->locked && lock->cpu == mycpu();
}
#endif

#if LOCKSTAT
LIST_HEAD(lockstat_list, klockstat);
static struct lockstat_list lockstat_list = { (struct klockstat*) NULL };
static struct spinlock lockstat_lock = { 
 locked: 0,
#if SPINLOCK_DEBUG
 name: "lockstat",
 cpu: (struct cpu*) NULL,
#endif
};

klockstat::klockstat(const char *name) : 
  rcu_freed("klockstat")
{
  magic = LOCKSTAT_MAGIC;
  memset(&s, 0, sizeof(s));
  safestrcpy(s.name, name, sizeof(s.name));
};

void
lockstat_init(struct spinlock *lk)
{
  lk->stat = new klockstat(lk->name);
  if (lk->stat == 0)
    return;

  acquire(&lockstat_lock);
  LIST_INSERT_HEAD(&lockstat_list, lk->stat, link);
  release(&lockstat_lock);
}

static void
lockstat_stop(struct spinlock *lk)
{
  if (lk->stat != NULL)
    lk->stat->magic = 0;
}

void
lockstat_clear(void)
{
  struct klockstat *stat, *tmp;

  acquire(&lockstat_lock);
  LIST_FOREACH_SAFE(stat, &lockstat_list, link, tmp) {
    if (stat->magic == 0) {
      LIST_REMOVE(stat, link);
      // So verifyfree doesn't follow le_next
      stat->link.le_next = 0;
      gc_delayed(stat);
    } else {
      memset(&stat->s.cpu, 0, sizeof(stat->s.cpu));
    }
  }
  release(&lockstat_lock);
}

static int
lockstat_read(struct inode *ip, char *dst, u32 off, u32 n)
{
  static const u64 sz = sizeof(struct lockstat);
  static struct {
    struct klockstat *stat;
    u32 off;
  } cache;

  struct klockstat *stat;
  u32 cur;

  if (off % sz || n < sz)
    return -1;

  acquire(&lockstat_lock);
  if (cache.off == off && cache.stat != NULL) {
    cur = cache.off;
    stat = cache.stat;
  } else {
    cur = 0;
    stat = LIST_FIRST(&lockstat_list);
  }
  for (; stat != NULL; stat = LIST_NEXT(stat, link)) {
    struct lockstat *ls = &stat->s;
    if (n < sizeof(*ls))
      break;
    if (cur >= off) {
      memmove(dst, ls, sz);
      dst += sz;
      n -= sz;
    }
    cur += sz;
  }
  release(&lockstat_lock);

  if (cur < off) {
    cache.off = 0;
    cache.stat = (struct klockstat*) NULL;
    return 0;
  }

  cache.off = cur;
  cache.stat = stat;
  return cur - off;
}

static int
lockstat_write(struct inode *ip, char *buf, u32 off, u32 n)
{
  int cmd = buf[0] - '0';

  switch(cmd) {
  case LOCKSTAT_START:
    lockstat_enable = 1;
    break;
  case LOCKSTAT_STOP:
    lockstat_enable = 0;
    break;
  case LOCKSTAT_CLEAR:
    lockstat_clear();
    break;
  default:
    return -1;
  }
  return n;
}

void
initlockstat(void)
{
  devsw[DEVLOCKSTAT].write = lockstat_write;
  devsw[DEVLOCKSTAT].read = lockstat_read;
}
#else
void
initlockstat(void)
{
}
#endif

void
initlock(struct spinlock *lk, const char *name, int lockstat)
{
#if SPINLOCK_DEBUG
  lk->name = name;
  lk->cpu = 0;
#endif
#if LOCKSTAT
  lk->stat = (struct klockstat*) NULL;
  if (lockstat)
    lockstat_init(lk);
#endif
  lk->locked = 0;
}

void
destroylock(struct spinlock *lk)
{
#if LOCKSTAT
  lockstat_stop(lk);
#endif
}

int
tryacquire(struct spinlock *lk)
{
  pushcli();
  locking(lk);
  if (xchg32(&lk->locked, 1) != 0) {
      popcli();
      return 0;
  }
  locked(lk, 0);
  return 1;
}

// Acquire the lock.
// Loops (spins) until the lock is acquired.
// Holding a lock for a long time may cause
// other CPUs to waste time spinning to acquire it.
void
acquire(struct spinlock *lk)
{
  u64 retries;

  pushcli();
  locking(lk);

  retries = 0;
  while(xchg32(&lk->locked, 1) != 0)
    retries++;
  locked(lk, retries);
}

// Release the lock.
void
release(struct spinlock *lk)
{
  releasing(lk);

  // The xchg serializes, so that reads before release are
  // not reordered after it.  The 1996 PentiumPro manual (Volume 3,
  // 7.2) says reads can be carried out speculatively and in
  // any order, which implies we need to serialize here.
  // But the 2007 Intel 64 Architecture Memory Ordering White
  // Paper says that Intel 64 and IA-32 will not move a load
  // after a store. So lock->locked = 0 would work here.
  // The xchg being asm volatile ensures gcc emits it after
  // the above assignments (and after the critical section).
  xchg32(&lk->locked, 0);

  popcli();
}
