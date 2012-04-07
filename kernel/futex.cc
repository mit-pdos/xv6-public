#include "types.h"
#include "kernel.hh"
#include "spinlock.h"
#include "cpputil.hh"
#include "ns.hh"
#include "errno.h"
#include "condvar.h"
#include "proc.hh"
#include "cpu.hh"

//
// futexkey
//
// XXX(sbw) this uses kernel addresses as keys and probably
// isn't complete.  For example, a user thread might wait
// on a user address, then another thread might unmap that
// address.

typedef u64* futexkey_t;

u64
futexkey_hash(futexkey_t const& key)
{
  return (u64)key;
}

u64
futexkey_val(futexkey_t const& key)
{
  return *key;
}

int
futexkey(const u64* useraddr, vmap* vmap, futexkey_t* key)
{
  u64* kaddr;

  kaddr = (u64*)pagelookup(vmap, (uptr)useraddr);
  if (kaddr == nullptr) {
    cprintf("futexkey: pagelookup failed\n");
    return -1;
  }
  *key = kaddr;
  return 0;
}

//
// futexaddr
//
struct futexaddr : public referenced, public rcu_freed
{
  futexaddr(futexkey_t key);
  virtual void do_gc();
  virtual void onzero() const;

  futexkey_t key_;
  bool inserted_;
  struct spinlock lock_;

  LIST_HEAD(proclist, proc) list_;

  NEW_DELETE_OPS(futexaddr);
};

xns<futexkey_t, futexaddr*, futexkey_hash> *nsfutex __mpalign__;

futexaddr::futexaddr(futexkey_t key)
  : rcu_freed("futexaddr"), key_(key), inserted_(false)
{
  initlock(&lock_, "futexaddr::lock_", LOCKSTAT_FUTEX);
  LIST_INIT(&list_);
}

void
futexaddr::do_gc(void)
{
  delete this;
}

void
futexaddr::onzero(void) const
{
  if (inserted_)
    assert(nsfutex->remove(key_, nullptr));
  gc_delayed((futexaddr*)this);
}

long
futexwait(futexkey_t key, u64 val, u64 timer)
{
  futexaddr* fa;

  {
    scoped_gc_epoch gc;
  again:
    fa = nsfutex->lookup(key);
    if (fa == nullptr) {
      fa = new futexaddr(key);
      if (nsfutex->insert(key, fa) < 0) {
        fa->dec();
        goto again;
      }
      fa->inserted_ = true;
    } else {
      if (!fa->tryinc()) {
        goto again;
      }
    }
  }
  assert(fa->key_ == key);

  acquire(&fa->lock_);  
  auto cleanup = scoped_cleanup([&fa](){
    release(&fa->lock_);
    fa->dec();
  });

  if (futexkey_val(fa->key_) != val)
    return -EWOULDBLOCK;
  LIST_INSERT_HEAD(&fa->list_, myproc(), futex_link);

  u64 nsecto = timer == 0 ? 0 : timer+nsectime();
  cv_sleepto(&myproc()->cv, &fa->lock_, nsecto);

  LIST_REMOVE(myproc(), futex_link);
  return 0;
}

long
futexwake(futexkey_t key, u64 nwake)
{
  futexaddr* fa;
  u64 nwoke = 0;
  proc* p;

  scoped_gc_epoch gc;
  fa = nsfutex->lookup(key);
  if (fa == nullptr)
    return 0;
  acquire(&fa->lock_);
  LIST_FOREACH(p, &fa->list_, futex_link) {
    if (nwoke >= nwake)
      break;
    cv_wakeup(&p->cv);
    nwoke++;
  }
  release(&fa->lock_);
  return 0;
}

void
initfutex(void)
{
  nsfutex = new xns<futexkey_t, futexaddr*, futexkey_hash>(false);
  if (nsfutex == 0)
    panic("initfutex");
}
