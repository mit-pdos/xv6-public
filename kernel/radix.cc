#include "crange_arch.hh"
#include "radix.hh"

enum { radix_debug = 0 };

#define dprintf(...) do { if (radix_debug) cprintf(__VA_ARGS__); } while(0)

// Returns the index needed to reach |level| from |level+1|.
static u32
index(u64 key, u32 level)
{
  u64 idx = key >> (bits_per_level * level);
  idx &= (1 << bits_per_level) - 1;
  return idx;
}

// Returns the level we stopped at.
template<class CB>
u32
descend(u64 key, radix_ptr *n, u32 level, CB cb, bool create)
{
  static_assert(key_bits == bits_per_level * radix_levels,
                "for now, we only support exact multiples of bits_per_level");
  assert(n);

  radix_entry v = n->load();
  if (v.is_null() && create) {
    assert(v.state() == entry_unlocked);
    radix_node *new_rn = new radix_node();
    radix_entry cur = v;
    v = radix_entry(new_rn);
    do {
      if (!cur.is_null()) {
        assert(cur.is_node());
        v = cur;
        delete new_rn;
        break;
      }
    } while (!n->compare_exchange_weak(cur, v));
  }
  // Node isn't there. Just return.
  if (v.is_null()) {
    return level+1;
  }

  radix_node *rn = v.node();

  radix_ptr *vptr = &rn->child[index(key, level)];
  if (level == 0) {
    cb(vptr);
    return level;
  } else {
    return descend(key, vptr, level-1, cb, create);
  }
}

void
radix_entry::release()
{
  if (is_null()) return;
  if (is_node()) {
    node()->do_gc();
  } else if (state() != entry_dead) {
    // Only decref live pointers. Dead ones are part of pages which
    // were RCU-freed and no longer own references.
    elem()->decref();
  }
}

radix_node::~radix_node()
{
  for (int i = 0; i < (1<<bits_per_level); i++) {
    child[i].load().release();
  }
}

radix::~radix()
{
  root_.load().release();
}

radix_elem*
radix::search(u64 key)
{
  scoped_gc_epoch gc;
  radix_entry cur = root_.load();
  for (u32 level = radix_levels-1; level >= 0 && !cur.is_elem(); level--) {
    cur = cur.node()->child[index(key >> shift_, level)].load();
  }
  dprintf("%p: search(%lu) -> %p\n", this, key >> shift_, cur.elem());
  return cur.elem();
}

radix_range
radix::search_lock(u64 start, u64 size)
{
  return radix_range(this, start >> shift_, size >> shift_);
}

radix_range::radix_range(radix *r, u64 start, u64 size)
  : r_(r), start_(start), size_(size)
{
  for (u64 k = start_; k != start_ + size_; k++) {
    if (descend(k, &r_->root_, radix_levels-1, [](radix_ptr *v) {
          radix_entry cur = v->load();
          while (cur.state() == entry_locked ||
                 !v->compare_exchange_weak(cur, cur.with_state(entry_locked)))
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
    if (descend(k, &r_->root_, radix_levels-1, [](radix_ptr *v) {
          radix_entry cur = v->load();
          do {
            assert(cur.state() == entry_locked);
          } while (!v->compare_exchange_weak(cur, cur.with_state(entry_unlocked)));
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
  dprintf("%p: replace: [%lu, %lu) with %p\n", r_, start, start + size, val);

  assert(start >= start_);
  assert(start + size <= start_ + size_);

  for (u64 k = start; k != start + size; k++) {
    if (descend(k, &r_->root_, radix_levels-1, [val](radix_ptr *v) {
          radix_entry cur = v->load();
          do {
            assert(cur.state() == entry_locked);
          } while (!v->compare_exchange_weak(cur, radix_entry(val, entry_locked)));
          if (val)
            val->incref();
          if (!cur.is_null())
            cur.elem()->decref();
        }, true)) {
      panic("radix_range::replace");
    }
  }
}

radix_iterator::radix_iterator(const radix* r, u64 k)
  : r_(r), k_(k) {
  dprintf("%p: Made iterator with k = %lu\n", r_, k_);
  if (k_ != ~0ULL) {
    path_[radix_levels] = r_->root_.load();
    if (path_[radix_levels].is_elem())
      leaf_ = radix_levels; // Maybe best to not do this...
    else if (!find_first_leaf(radix_levels - 1))
      k_ = ~0ULL;
  }
  dprintf("%p: Adjusted: k = %lu\n", r_, k_);
}

bool
radix_iterator::advance(u32 level)
{
  // First, see if we can advance a lower level.
  if (level > leaf_ && advance(level-1)) {
    // Nothing more to do.
    return true;
  }

  // Try to advance this level, if we can.
  u32 start_idx = index(k_, level)+1;
  if (start_idx < (1<<bits_per_level)) {
    // Find the first leaf starting at our sibling node.
    k_ &= ~((1ULL<<((level+1) * bits_per_level)) - 1);
    k_ |= (u64(start_idx) << (level * bits_per_level));
    return find_first_leaf(level);
  } else {
    return false;
  }
}

bool
radix_iterator::find_first_leaf(u32 level)
{
  // Find the first non-empty node after k_ on this level.
  for (u32 idx = index(k_, level); idx < (1<<bits_per_level); idx++) {
    radix_entry next = node(level+1)->child[idx].load();
    if (!next.is_null()) {
      if (index(k_, level) != idx) {
        // We had to advance; clear everything this level and under
        // and set this one.
        k_ &= ~((1ULL<<((level+1) * bits_per_level)) - 1);
        k_ |= (u64(idx) << (level * bits_per_level));
      }
      path_[level] = next;

      if (next.is_elem()) {
        // Found a leaf. Stop now.
        leaf_ = level;
        return true;
      } else if (find_first_leaf(level-1))
        // Keep looking.
        return true;
    }
  }

  // Failed to find a leaf. Abort.
  return false;
}
