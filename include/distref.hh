// Distributed, lazily-reconciled reference counters.

#pragma once

#include "types.h"
#include "atomic.hh"
#include "percpu.hh"

class distributed_refcnt;

constexpr int distref_gen_bits = 24;

void distref_check(distributed_refcnt *dr);

struct distref_counter
{
  // The lower distref_gen_bits bits of this are a monotonically
  // increasing generation count.  The remaining bits are the actual
  // reference count.
  std::atomic<s64> count_gen;

  // List link for this core's maybe-free list.
  distributed_refcnt *next;

  constexpr distref_counter()
    : count_gen(0), next(nullptr) { }
};

// Base class for objects managed by a distributed, lazily-reconciled
// reference counter.  Subclasses should override distref_free and
// provide methods to invoke distref_inc and distref_dec.
class distributed_refcnt
{
public:
  distributed_refcnt()
    : ref_(), next_free_(nullptr) { }
  distributed_refcnt(const distributed_refcnt &o) = delete;
  virtual ~distributed_refcnt() = 0;

  distributed_refcnt &operator=(const distributed_refcnt &o) = delete;

protected:
  void distref_inc(u64 delta = 1)
  {
    ref_.get_unchecked()->count_gen += (delta << distref_gen_bits) + 1;
  }

  void distref_dec(u64 delta = 1)
  {
    s64 add = ((-delta) << distref_gen_bits) + 1;

    // count is atomic, so we can do this with interrupts enabled
    if ((ref_.get_unchecked()->count_gen += add) <= 0)
      distref_check(this);
  }

private:
  virtual void distref_free() = 0;

  friend class distref_worker;
  percpu<distref_counter> ref_;
  std::atomic<distributed_refcnt*> next_free_;
};
