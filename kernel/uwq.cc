//
// XXX
//  - vmap doesn't need to be rcu_freed anymore
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
      if (uwq->trywork() == 1)
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
    uentry_(0), ustack_(UWQSTACK)
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
  finishworkers();
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

int
uwq::trywork(void)
{
  struct proc* p;

  p = getworker();
  if (p == nullptr)
    return -1;

  // XXX(sbw) filetable, etc
  p->cpuid = mycpuid();

  acquire(&p->lock);
  addrun(p);
  release(&p->lock);

  cprintf("trying to run..\n");
  return 1;
}

void
uwq::finishworkers(void)
{
  for (int i = 0; i < NCPU; i++)
    if (worker_[i].proc != nullptr)
      panic("uwq::finishworkers");
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

  vmap_->incref();
  ftable_->incref();
  // finishproc will drop these refs
  
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
  cprintf("stacktop %lx\n", stacktop);
  p->tf->rip = uentry;
  p->tf->cs = UCSEG | 0x3;
  p->tf->ds = UDSEG | 0x3;
  p->tf->ss = p->tf->ds;
  p->tf->rflags = FL_IF;

  return p;
}

proc*
uwq::getworker(void)
{
  int slot = -1;

  scoped_acquire lockx(&lock_);

  for (int i = 0; i < NCPU; i++) {
    if (worker_[i].running)
      continue;
    if (worker_[i].proc != nullptr) {
      worker_[i].running = true;
      return worker_[i].proc;
    } else if (slot == -1) {
      slot = i;
    }
  }

  if (slot != -1) {
    proc* p = allocworker();
    if (p != nullptr) {
      worker_[slot].proc = p;
      worker_[slot].running = true;
      return worker_[slot].proc;
    }
  }
    
  return nullptr;
}
