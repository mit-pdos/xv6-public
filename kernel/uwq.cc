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
    padded_length *len = p->vmap->uwq_len_;

    if (len == nullptr)
      break;

    if (uwq_length(len).haswork()) {
      // XXX(sbw) start a worker thread..
      break;
    }
  }

  return 0;
}
