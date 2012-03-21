//
// XXX
//  - workers should have a uwq
//  - pin workers

#include "types.h"
#include "amd64.h"
#include "kernel.hh"
#include "cpu.hh"
#include "gc.hh"
#include "percpu.hh"
#include "spinlock.h"
#include "condvar.h"
#include "proc.hh"
#include "uwq.hh"
#include "vm.hh"
#include "kalloc.hh"
#include "bits.hh"
extern "C" {
#include "kern_c.h"
}

int
uwq_trywork(void)
{
  u64 i, k;

  // A "random" victim CPU
  k = rdtsc();
  for (i = 0; i < NCPU; i++) {
    u64 j = (i+k) % NCPU;

    if (j == mycpuid())
      continue;
    struct cpu *c = &cpus[j];

    scoped_gc_epoch xgc();
    barrier();
    struct proc *p = c->proc;
    if (p == nullptr || p->uwq == nullptr)
      continue;
    uwq* uwq = p->uwq;

    if (uwq->haswork()) {
      if (uwq->trywork())
        return 1;
      // XXX(sbw) start a worker thread..
      break;
    }
  }

  return 0;
}

long
sys_wqwait(void)
{
  uwq_worker* w = myproc()->worker;
  if (w == nullptr)
    return -1;

  return w->wait();
}

//
// uwq_worker
//
uwq_worker::uwq_worker(uwq* u, proc* p)
  : uwq_(u), proc_(p), running_(false)
{
  initlock(&lock_, "worker_lock", 0);
  initcondvar(&cv_, "worker_cv");
}

long
uwq_worker::wait(void)
{
  acquire(&lock_);
  uwq_->tryexit(this);
  running_ = false;
  cv_sleep(&cv_, &lock_);
  uwq_->tryexit(this);
  release(&lock_);
  return 0;
}

//
// uwq
//
uwq*
uwq::alloc(vmap* vmap, filetable *ftable)
{
  padded_length* len;
  uwq* u;

  len = (padded_length*) ksalloc(slab_userwq);  
  if (len == nullptr)
    return nullptr;

  ftable->incref();
  vmap->incref();

  u = new uwq(vmap, ftable, len);
  if (u == nullptr) {
    ftable->decref();
    vmap->decref();
    ksfree(slab_userwq, len);
    return nullptr;
  }
  u->inc();

  if (mapkva(vmap->pml4, (char*)len, USERWQ, USERWQSIZE)) {
    ftable->decref();
    vmap->decref();
    ksfree(slab_userwq, len);
    u->dec();
    return nullptr;
  }

  return u;
}

uwq::uwq(vmap* vmap, filetable *ftable, padded_length *len) 
  : rcu_freed("uwq"),
    vmap_(vmap), ftable_(ftable), len_(len),
    uentry_(0), ustack_(UWQSTACK), uref_(0)
{
  for (int i = 0; i < NCPU; i++)
    len_[i].v_ = 0;

  initlock(&lock_, "uwq_lock", 0);
  memset(worker_, 0, sizeof(worker_));
}

uwq::~uwq(void)
{ 
  if (len_ != nullptr)
    ksfree(slab_userwq, len_);
  vmap_->decref();
  ftable_->decref();
}

void
uwq::tryexit(uwq_worker *w)
{
  if (ref() == 0) {
    if (--uref_ == 0)
      gc_delayed(this);
    release(&w->lock_);
    delete w;
    exit();
  }
}

bool
uwq::haswork(void)
{
  if (len_ == nullptr)
    return false;

  for (int i = 0; i < NCPU; i++) {
    if (len_[i].v_ > 0) {
      return true;
    }
  }
  return false;
}

bool
uwq::trywork(void)
{
  scoped_acquire lock0(&lock_);

  if (ref() == 0)
    return false;

  int slot = -1;
  for (int i = 0; i < NCPU; i++) {
    if (worker_[i] == nullptr) {
      if (slot == -1)
        slot = i;
      continue;
    }

    uwq_worker *w = worker_[i];
    if (w->running_)
      continue;
    else {
      scoped_acquire lock1(&w->lock_);
      proc* p = w->proc_;

      cprintf("uwq::trywork: untested\n");
      acquire(&p->lock);
      p->cpuid = mycpuid();
      release(&p->lock);

      w->running_ = true;
      cv_wakeup(&w->cv_);
      return true;
    }
  }

  if (slot != -1) {
    proc* p = allocworker();
    if (p != nullptr) {
      uwq_worker* w = new uwq_worker(this, p);
      assert(w != nullptr);

      ++uref_;
      p->worker = w;
      w->running_ = true;

      acquire(&p->lock);
      p->cpuid = mycpuid();
      addrun(p);
      release(&p->lock);

      worker_[slot] = w;
      return true;
    }
  }
    
  return nullptr;
}

void
uwq::finish(void)
{
  bool gcnow = true;

  scoped_acquire lock0(&lock_);
  for (int i = 0; i < NCPU; i++) {
    if (worker_[i] != nullptr) {
      uwq_worker* w = worker_[i];
      gcnow = false;
      acquire(&w->lock_);
      cv_wakeup(&w->cv_);
      release(&w->lock_);
    }
  }
  
  if (gcnow)
    gc_delayed(this);
}

void
uwq::onzero() const
{
  uwq *u = (uwq*)this;
  u->finish();
}

void*
uwq::buffer(void)
{
  return (void*)len_;
}

void
uwq::setuentry(uptr uentry)
{
  uentry_ = uentry;
}

proc*
uwq::allocworker(void)
{
  uptr uentry = uentry_;

  if (uentry == 0)
    return nullptr;

  proc* p = allocproc();
  if (p == nullptr)
    return nullptr;
  safestrcpy(p->name, "uwq_worker", sizeof(p->name));

  // finishproc will drop these refs
  vmap_->incref();
  ftable_->incref();
  
  p->vmap = vmap_;
  p->ftable = ftable_;
    
  struct vmnode *vmn;
  if ((vmn = new vmnode(UWQSTACKPAGES)) == nullptr) {
    finishproc(p);
    return nullptr;
  }

  uptr stacktop = ustack_ + (UWQSTACKPAGES*PGSIZE);
  if (vmap_->insert(vmn, ustack_, 1) < 0) {
    delete vmn;
    finishproc(p);
    return nullptr;
  }
  ustack_ += (UWQSTACKPAGES*PGSIZE);

  p->tf->rsp = stacktop - 8;
  p->tf->rip = uentry;
  p->tf->cs = UCSEG | 0x3;
  p->tf->ds = UDSEG | 0x3;
  p->tf->ss = p->tf->ds;
  p->tf->rflags = FL_IF;

  return p;
}
