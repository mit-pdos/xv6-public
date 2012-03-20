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
long
uwq_worker::wait(void)
{
  acquire(&lock);
  uwq->tryexit(this);
  cv_sleep(&cv, &lock);
  uwq->tryexit(this);
  release(&lock);
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

  // XXX(sbw) move to uwq_worker constructor
  for (int i = 0; i < NCPU; i++) {
    initlock(&worker_[i].lock, "worker_lock", 0);
    initcondvar(&worker_[i].cv, "worker_cv");
    worker_[i].uwq = this;
    worker_[i].running = false;
    worker_[i].proc = nullptr;
  }
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
    release(&w->lock);
    w->proc = nullptr;
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
    if (worker_[i].running)
      continue;
    else if (worker_[i].proc != nullptr) {
      scoped_acquire lock1(&worker_[i].lock);
      proc* p = worker_[i].proc;

      panic("uwq::trywork: untested");
      acquire(&p->lock);
      p->cpu_pin = 1;
      p->cpuid = mycpuid();
      release(&p->lock);

      worker_[i].running = true;
      cv_wakeup(&worker_[i].cv);
      return true;
    } else if (slot == -1) {
      slot = i;
    }
  }

  if (slot != -1) {
    proc* p = allocworker();
    if (p != nullptr) {
      ++uref_;
      p->worker = &worker_[slot];
      worker_[slot].proc = p;
      worker_[slot].running = true;

      acquire(&p->lock);
      p->cpu_pin = 1;
      p->cpuid = mycpuid();
      addrun(p);
      release(&p->lock);

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
    if (worker_[i].proc != nullptr) {
      gcnow = false;
      acquire(&worker_[i].lock);
      cv_wakeup(&worker_[i].cv);
      release(&worker_[i].lock);
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
