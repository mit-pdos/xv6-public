#include "types.h"
#include "amd64.h"
#include "kernel.hh"
#include "cpu.hh"
#include "gc.hh"
#include "percpu.hh"
#include "spinlock.h"
#include "condvar.h"
#include "uwq.hh"
#include "proc.hh"
#include "vm.hh"
#include "kalloc.hh"

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

    scoped_gc_epoch gcx();
    barrier();
    struct proc *p = c->proc;
    if (p == nullptr || p->vmap == nullptr)
      continue;
    uwq* uwq = &p->vmap->uwq_;

    if (uwq->haswork()) {
      uwq->trywork();
      // XXX(sbw) start a worker thread..
      break;
    }
  }

  return 0;
}

//
// uwq
//
uwq::uwq(padded_length *len) 
  : len_(len)
{
  if (len_ != nullptr) {
    for (int i = 0; i < NCPU; i++)
        len_[i].v_ = 0;
  } else {
    cprintf("uwq::uwq: nullptr len\n");
  }

  initlock(&lock_, "uwq_lock", 0);
  memset(worker_, 0, sizeof(worker_));
}

uwq::~uwq(void)
{ 
  if (len_ != nullptr)
    ksfree(slab_userwq, len_);
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

  return 0;
}

void*
uwq::buffer(void)
{
  return (void*)len_;
}

proc*
uwq::getworker(void)
{
  int slot = -1;

  scoped_acquire lockx(&lock_);

  for (int i = 0; i < NCPU; i++) {
    if (worker_[i].proc != nullptr) {
      worker_[i].running = 1;
      return worker_[i].proc;
    } else if (slot == -1) {
      slot = i;
    }
  }

  if (slot != -1) {
    
    panic("XXX");
  }

  return nullptr;
}
