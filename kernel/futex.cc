#include "types.h"
#include "kernel.hh"
#include "spinlock.h"
#include "cpputil.hh"
#include "ns.hh"
#include "errno.h"
#include "condvar.h"
#include "proc.hh"
#include "cpu.hh"
#include "percpu.hh"
#include "kmtrace.hh"
#include "sperf.hh"

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
// nscache
//
struct nscache {
  struct spinlock lock_;
  volatile u64 head_;
  volatile u64 tail_;
  xns<u32, proc*, proc::hash>* ns_[16];

  nscache();
  xns<u32, proc*, proc::hash>* alloc();
  bool cache(xns<u32, proc*, proc::hash>* ns);

  NEW_DELETE_OPS(nscache);
};

percpu<nscache> nscache_;

nscache::nscache(void)
{
  initlock(&lock_, "nscache::lock_", LOCKSTAT_FUTEX);
  head_ = 0;
  tail_ = 0;
}

xns<u32, proc*, proc::hash>*
nscache::alloc(void)
{
  xns<u32, proc*, proc::hash>* ns = nullptr;
  
  acquire(&lock_);
  if (head_ - tail_ > 0) {
    u64 i = tail_%NELEM(ns_);
    ++tail_;
    ns = ns_[i];
  }
  release(&lock_);
  return ns;
}

bool
nscache::cache(xns<u32, proc*, proc::hash>* ns)
{
  bool cached = false;

  acquire(&lock_);
  if (head_ - tail_ < NELEM(ns_)) {
    u64 i = head_%NELEM(ns_);
    ++head_;
    ns_[i] = ns;
    cached = true;
  }
  release(&lock_);
  return cached;
}

//
// futexaddr
//
struct futexaddr : public referenced, public rcu_freed
{
  static futexaddr* alloc(futexkey_t key);

  virtual void do_gc();
  virtual void onzero() const;

  futexkey_t key_;
  bool inserted_;
  xns<u32, proc*, proc::hash>* const nspid_;

private:
  futexaddr(futexkey_t key, xns<u32, proc*, proc::hash>* nspid);
  NEW_DELETE_OPS(futexaddr);
};

xns<futexkey_t, futexaddr*, futexkey_hash> *nsfutex __mpalign__;

futexaddr*
futexaddr::alloc(futexkey_t key)
{
  xns<u32, proc*, proc::hash>* nspid;
  futexaddr* fa;

  nspid = nscache_->alloc();
  if (nspid == nullptr)
    nspid = new xns<u32, proc*, proc::hash>(false);

  if (nspid == nullptr)
    return nullptr;
  fa = new futexaddr(key, nspid);
  if (fa == nullptr) {
    delete nspid;
    return nullptr;
  }

  return fa;
}

futexaddr::futexaddr(futexkey_t key, xns<u32, proc*, proc::hash>* nspid)
  : rcu_freed("futexaddr"), key_(key), inserted_(false), nspid_(nspid)
{
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
  // Normally deallocate members in the destructor, but in this case
  // we don't want to wait for the gc to fill the cache
  if (!nscache_->cache(nspid_))
    delete nspid_;
  gc_delayed((futexaddr*)this);
}

long
futexwait(futexkey_t key, u64 val, u64 timer)
{
  ANON_REGION(__func__, &perfgroup);

  futexaddr* fa;

  mtreadavar("futex:ns:%p", key);
  {
    scoped_gc_epoch gc;
  again:
    fa = nsfutex->lookup(key);
    if (fa == nullptr) {
      fa = futexaddr::alloc(key);
      if (fa == nullptr) {
        cprintf("futexwait futexaddr::alloc failed\n");
        return -1;
      }
      if (nsfutex->insert(key, fa) < 0) {
        fa->dec();
        goto again;
      }
      mtwriteavar("futex:ns:%p", key);
      fa->inserted_ = true;
    } else {
      if (!fa->tryinc()) {
        goto again;
      }
    }
  }
  assert(fa->key_ == key);
  mtwriteavar("futex:%p.%p", key, fa);

  acquire(&myproc()->futex_lock);  
  auto cleanup = scoped_cleanup([&fa](){
    release(&myproc()->futex_lock);
    fa->dec();
  });

  // This first check is an optimization
  if (futexkey_val(fa->key_) != val)
    return -EWOULDBLOCK;

  if (fa->nspid_->insert(myproc()->pid, myproc()) < 0)
    return -1;

  if (futexkey_val(fa->key_) != val) {
    fa->nspid_->remove(myproc()->pid, nullptr);
    return -EWOULDBLOCK;
  }

  u64 nsecto = timer == 0 ? 0 : timer+nsectime();
  cv_sleepto(&myproc()->cv, &myproc()->futex_lock, nsecto);

  assert(fa->nspid_->remove(myproc()->pid, nullptr));
  return 0;
}

long
futexwake(futexkey_t key, u64 nwake)
{
  ANON_REGION(__func__, &perfgroup);

  futexaddr* fa;
  u64 nwoke = 0;

  if (nwake == 0)
    return -1;

  mtreadavar("futex:ns:%p", key);
  {
    scoped_gc_epoch gc;
    fa = nsfutex->lookup(key);
    if (fa == nullptr || !fa->tryinc())
      return 0;
  }

  auto cleanup = scoped_cleanup([&fa](){
    fa->dec();
  });
  mtwriteavar("futex:%p.%p", key, fa);

  fa->nspid_->enumerate([&nwoke, &nwake](u32 pid, proc* p) {
    acquire(&p->futex_lock);
    cv_wakeup(&p->cv);
    release(&p->futex_lock);
    ++nwoke;
    if (nwoke >= nwake)
      return 1;
    return 0;
  });

  return 0;
}

void
initfutex(void)
{
  nsfutex = new xns<futexkey_t, futexaddr*, futexkey_hash>(false);
  if (nsfutex == 0)
    panic("initfutex");
}
