#include "crange_arch.hh"
#include "radix.hh"

// Returns the level we stopped at.
template<class CB>
u32
descend(u64 key, markptr<void> *n, u32 level, CB cb, bool create)
{
  static_assert(key_bits == bits_per_level * radix_levels,
                "for now, we only support exact multiples of bits_per_level");
  assert(n);

  void *v = n->ptr();
  if (v == 0 && create) {
    radix_node *new_rn = new radix_node();
    if (n->ptr().cmpxch_update(&v, (void*) new_rn))
      v = new_rn;
    else
      delete new_rn;
  }
  // Node isn't there. Just return.
  if (v == 0) {
    return level+1;
  }

  radix_node *rn = (radix_node*) v;

  u64 idx = key >> (bits_per_level * level);
  idx &= (1<<bits_per_level)-1;
  markptr<void> *vptr = &rn->ptr[idx];
  if (level == 0) {
    cb(vptr);
    return level;
  } else {
    return descend(key, vptr, level-1, cb, create);
  }
}

radix_elem*
radix::search(u64 key)
{
  radix_elem *result = 0;
  descend(key >> shift_, &root_, radix_levels-1, [&result](markptr<void> *v) {
      result = (radix_elem*) v->ptr().load();
    }, false);
  return result;
}

radix_range
radix::search_lock(u64 start, u64 size)
{
  return radix_range(this, start >> shift_, size >> shift_);
}

u64
radix::skip_empty(u64 k) const
{
  u64 next_k = k;
  while (next_k < (1UL<<key_bits)) {
    // Does next_k exist?
    // FIXME: evil evil const_cast
    u32 level = descend(next_k, const_cast<markptr<void>*>(&root_),
                        radix_levels-1, [](markptr<void> *v){}, false);
    if (level == 0) {
      return next_k;
    }
    u64 mask = 1UL<<(bits_per_level * level);
    // Skip past everything we know is missing.
    next_k = (next_k & ~(mask-1)) + mask;
  }
  // Nope, no successor.
  return ~0ULL;
}

radix_range::radix_range(radix *r, u64 start, u64 size)
  : r_(r), start_(start), size_(size)
{
  for (u64 k = start_; k != start_ + size_; k++) {
    if (descend(k, &r_->root_, radix_levels-1, [](markptr<void> *v) {
          while (!v->mark().xchg(true))
            ; // spin
        }, true) != 0) {
      panic("radix_range");
    }
  }
}

radix_range::~radix_range()
{
  if (!r_)
    return;

  for (u64 k = start_; k != start_ + size_; k++) {
    if (descend(k, &r_->root_, radix_levels-1, [](markptr<void> *v) {
          v->mark() = false;
        }, true) != 0) {
      panic("~radix_range");
    }
  }
}

void
radix_range::replace(u64 start, u64 size, radix_elem *val)
{
  start = start >> r_->shift_;
  size = size >> r_->shift_;

  assert(start >= start_);
  assert(start + size <= start_ + size_);

  for (u64 k = start; k != start + size; k++) {
    if (descend(k, &r_->root_, radix_levels-1, [val](markptr<void> *v) {
          void* cur = v->ptr().load();
          while (!v->ptr().cmpxch_update(&cur, val))
            ; // spin
          if (val)
            val->incref();
          if (cur)
            ((radix_elem*) cur)->decref();
        }, true)) {
      panic("radix_range::replace");
    }
  }
}

radix_elem*
radix_iterator::operator*()
{
  radix_elem *result = 0;
  descend(k_, (markptr<void>*) &r_->root_, radix_levels-1, [&result](markptr<void> *v) {
      result = (radix_elem*) v->ptr().load();
    }, false);
  return result;
}
