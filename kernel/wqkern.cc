#include "types.h"
#include "kernel.hh"
#include "spinlock.h"
#include "amd64.h"
#include "cpu.hh"
#include "kalloc.hh"
#include "wq.hh"

static wq *wq_;
static wq *wqcrit_;

void*
xallocwork(unsigned long nbytes)
{
  return kmalloc(nbytes, "xallocwork");
}

void 
xfreework(void* ptr, unsigned long nbytes)
{
  kmfree(ptr, nbytes);
}

size_t
wq_size(void)
{
  return sizeof(wq);
}

int
wq_push(work *w)
{
  assert(wq_);
  return wq_->push(w, mycpuid());
}

void
wqcrit_trywork(void)
{
  assert(wqcrit_);
  while (wqcrit_->trywork(false))
    ;
}

int
wqcrit_push(work *w, int c)
{
  assert(wqcrit_);
  if (c >= NCPU || c < 0)
    panic("Weird CPU %d", c);
  return wqcrit_->push(w, c);
}

int
wq_trywork(void)
{
  assert(wq_ && wqcrit_);
  return wqcrit_->trywork(false) || wq_->trywork(true);
}

void
wq_dump(void)
{
  if (wq_)
    return wq_->dump();
}

void
initwq(void)
{
  wq_ = new wq();
  wqcrit_ = new wq();
  if (wq_ == nullptr || wqcrit_ == nullptr)
    panic("initwq");
}
