#include "types.h"
#include "amd64.h"
#include "kernel.hh"
#include "cpu.hh"
#include "uwq.hh"
#include "gc.hh"
#include "percpu.hh"
#include "spinlock.h"
#include "condvar.h"
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

    scoped_gc_epoch();
    struct proc *p = c->proc;
    if (p == nullptr || p->vmap == nullptr)
      continue;
    uwq* uwq = &p->vmap->uwq_;

    if (uwq->haswork()) {
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

void*
uwq::buffer(void)
{
  return (void*)len_;
}
