#include "types.h"
#include "atomic.hh"
#include "spinlock.h"
#include "kernel.hh"
#include "cpputil.hh"
#include "radix.hh"

template<class CB>
void
descend(u64 key, markptr<void> *n, u32 level, CB cb)
{
  // for now, we only support exact multiples of bits_per_level
  assert(key_bits == bits_per_level * radix_levels);
  assert(n);

  void *v = n->ptr();
  if (v == 0) {
    radix_node *new_rn = new radix_node();
    if (n->ptr().cmpxch_update(&v, (void*) new_rn))
      v = new_rn;
    else
      delete new_rn;
  }

  radix_node *rn = (radix_node*) v;

  u64 idx = key >> (bits_per_level * level);
  idx &= (1<<bits_per_level)-1;
  markptr<void> *vptr = &rn->ptr[idx];
  if (level == 0)
    cb(vptr);
  else
    descend(key, vptr, level-1, cb);
}

radix_elem*
radix::search(u64 key)
{
  radix_elem *result = 0;
  descend(key >> shift_, &root_, radix_levels-1, [&result](markptr<void> *v) {
      result = (radix_elem*) v->ptr().load();
    });
  return result;
}

radix_range
radix::search_lock(u64 start, u64 size)
{
  return radix_range(this, start >> shift_, size >> shift_);
}

radix_range::radix_range(radix *r, u64 start, u64 size)
  : r_(r), start_(start), size_(size)
{
  for (u64 k = start_; k != start_ + size_; k++)
    descend(k, &r_->root_, radix_levels-1, [](markptr<void> *v) {
        while (!v->mark().xchg(true))
          ; // spin
      });
}

radix_range::~radix_range()
{
  if (!r_)
    return;

  for (u64 k = start_; k != start_ + size_; k++)
    descend(k, &r_->root_, radix_levels-1, [](markptr<void> *v) {
        v->mark() = false;
      });
}

void
radix_range::replace(u64 start, u64 size, radix_elem *val)
{
  start = start >> r_->shift_;
  size = size >> r_->shift_;

  assert(start >= start_);
  assert(start + size <= start_ + size_);

  for (u64 k = start; k != start + size; k++)
    descend(k, &r_->root_, radix_levels-1, [val](markptr<void> *v) {
        void* cur = v->ptr().load();
        while (!v->ptr().cmpxch_update(&cur, val))
          ; // spin
        val->incref();
        if (cur)
          ((radix_elem*) cur)->decref();
      });
}

radix_elem*
radix_iterator::operator*()
{
  radix_elem *result = 0;
  descend(k_, (markptr<void>*) &r_->root_, radix_levels-1, [&result](markptr<void> *v) {
      result = (radix_elem*) v->ptr().load();
    });
  return result;
}
