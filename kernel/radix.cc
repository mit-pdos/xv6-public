#include "crange_arch.hh"
#include "radix.hh"

enum { radix_debug = 0 };

#define dprintf(...) do { if (radix_debug) cprintf(__VA_ARGS__); } while(0)

static_assert(key_bits == bits_per_level * radix_levels,
              "for now, we only support exact multiples of bits_per_level");

// Returns the index needed to reach |level| from |level+1|.
static u32
index(u64 key, u32 level)
{
  u64 idx = key >> (bits_per_level * level);
  idx &= (1 << bits_per_level) - 1;
  return idx;
}

// Returns the size of a subtree for a node at |level|.
static u64
level_size(u32 level)
{
  return 1L << (bits_per_level * level);
}

static radix_entry
push_down(radix_entry cur, radix_ptr *ptr)
{
  while (cur.state() != entry_dead && cur.state() != entry_node) {
    // If we're locked, just spin and try again.
    if (cur.state() == entry_locked) {
      cur = ptr->load();
      continue;
    }

    // Make a new node.
    assert(cur.state() == entry_unlocked);
    radix_elem *elem = cur.elem();
    radix_node *new_rn = new radix_node();
    if (elem != nullptr) {
      for (int i = 0; i < (1<<bits_per_level); i++) {
        new_rn->child[i].store(radix_entry(elem));
      }
      elem->incref(1<<bits_per_level);
    }
    if (ptr->compare_exchange_weak(cur, radix_entry(new_rn))) {
      // Release the ref from the pointer we replaced. FIXME: Bouncing
      // on the reference count here is annoying. Maybe the reference
      // count should be dependent on the high of the leaf?
      if (elem != nullptr)
        elem->decref();
    } else {
      // Someone else beat us to it. Back out. FIXME: docs say
      // compare_exchange_weak can return spuriously. Should avoid
      // reallocating new_rn if elem doesn't change.

      // Avoid bouncing on the refcount 1<<bits_per_level times.
      if (elem != nullptr) {
        for (int i = 0; i < (1<<bits_per_level); i++) {
          new_rn->child[i].store(radix_entry(nullptr));
        }
        elem->decref(1<<bits_per_level);
      }

      new_rn->do_gc();
    }
  }
  return cur;
}

// Returns the next node to be processed, whether or not it falls in
// the range. Success is to return cur_start + cur_size. Otherwise we
// stopped early and bubble up the error.
template <class CB>
u64
update_range(radix_entry cur, radix_ptr *ptr, CB cb,
             u64 cur_start, u64 cur_end,
             u64 start, u64 end, u32 level = radix_levels)
{
  assert(level_size(level) == cur_end - cur_start);
  // If ranges are disjoint, do nothing. We manage to process everyone
  // for free.
  if (cur_start >= end || start >= cur_end)
    return cur_end;

  // If our range is not strictly contained in the target, ensure we
  // are at a node.
  if (start > cur_start || end < cur_end) {
    cur = push_down(cur, ptr);
    // Failed. Next time resume at cur_start.
    if (cur.state() == entry_dead)
      return cur_start;
  }

  if (cur.is_node()) {
    // Find the place to start.
    if (start < cur_start)
      start = cur_start;
    assert(level > 0);
    int i = index(start, level - 1);
    u64 child_size = (cur_end - cur_start) >> bits_per_level;
    u64 child_start = cur_start + i * child_size;
    for (; (i < (1<<bits_per_level)) && (child_start < cur_end);
         i++, child_start += child_size) {
      radix_ptr *child = &cur.node()->child[i];
      u64 ret = update_range(child->load(), child, cb,
                             child_start, child_start + child_size,
                             start, end, level - 1);
      if (ret != child_start + child_size) return ret;
    }
    return cur_end;
  } else {
    // If we're here, the target range must completely contain this
    // element.
    assert(start <= cur_start && cur_end <= end);
    // Callback returns how far it processed.
    return cb(cur, ptr, cur_start, cur_end, level);
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
  dprintf("%p: search(%lx) -> %p\n", this, key >> shift_, cur.elem());
  return cur.elem();
}

radix_range
radix::search_lock(u64 start, u64 size)
{
  return radix_range(this, start >> shift_, size >> shift_);
}

// This should be a lambda, but it's awkward for a lambda to call
// itself.
struct entry_locker {
  u64 start_;
  u64 end_;

  entry_locker(u64 start, u64 end) : start_(start), end_(end) { }
  u64 operator()(radix_entry cur, radix_ptr *ptr, u64 cur_start, u64 cur_end, u32 level) const {
    while (cur.state() != entry_dead && cur.state() != entry_node) {
      // Locked -> spin and try again.
      if (cur.state() == entry_locked) {
        cur = ptr->load();
        continue;
      }
      // Otherwise it's unlocked. Try to load it.
      if (ptr->compare_exchange_weak(cur, cur.with_state(entry_locked))) {
        // Success. Remember the current value and break out.
        cur = cur.with_state(entry_locked);
        break;
      }
    }
    // Someone deleted this leaf. Abort this iteration.
    if (cur.state() == entry_dead)
      return cur_start;
    // Someone pushed down. Recurse some more.
    if (cur.state() == entry_node)
      return update_range(cur, ptr, *this, cur_start, cur_end, start_, end_, level-1);
    // We managed to lock!
    assert(cur.state() == entry_locked);
    return cur_end;
  }
};

radix_range::radix_range(radix *r, u64 start, u64 size)
  : r_(r), start_(start), size_(size)
{
  u64 next_start = start_;
  u64 end = start_ + size_;
  // Lock the range from left to right. If we hid a dead element re-load the root.
  while (next_start < end) {
    const entry_locker& cb = entry_locker(next_start, end);
    next_start = update_range(r_->root_.load(), &r_->root_, cb,
                              0, 1L << key_bits, next_start, end);
    assert(next_start >= start_);
  }
}

radix_range::~radix_range()
{
  if (!r_)
    return;

  u64 ret = update_range(r_->root_.load(), &r_->root_, [](radix_entry cur, radix_ptr *ptr, u64 cur_start, u64 cur_end, u32 level) -> u64 {
      do {
        // It had better still be locked.
        assert(cur.state() == entry_locked);
      } while (!ptr->compare_exchange_weak(cur, cur.with_state(entry_unlocked)));
      return cur_end;
    }, 0, 1L << key_bits, start_, start_ + size_);
  // Impossible to hit entry_dead. We own the lock.
  if (ret != 1L << key_bits)
    panic("~radix_range");
}

void
radix_range::replace(u64 start, u64 size, radix_elem *val)
{
  start = start >> r_->shift_;
  size = size >> r_->shift_;
  dprintf("%p: replace: [%lx, %lx) with %p\n", r_, start, start + size, val);

  assert(start >= start_);
  assert(start + size <= start_ + size_);

  u64 ret = update_range(r_->root_.load(), &r_->root_, [val](radix_entry cur, radix_ptr *ptr, u64 cur_start, u64 cur_end, u32 level) -> u64 {
      dprintf(" -> [%lx, %lx); size = %lx\n", cur_start, cur_end, cur_end - cur_start);
      do {
        assert(cur.state() == entry_locked);
      } while (!ptr->compare_exchange_weak(cur, radix_entry(val, entry_locked)));
      if (val)
        val->incref();
      if (!cur.is_null())
        cur.elem()->decref();
      return cur_end;
    }, 0, 1L << key_bits, start, start + size);
  // Impossible to hit entry_dead. We own the lock.
  if (ret != 1L << key_bits)
    panic("radix_range::replace");

  // TODO: If we can, collapse some intermediate nodes, RCU-freeing
  // them.
}

radix_iterator::radix_iterator(const radix* r, u64 k)
  : r_(r), k_(k) {
  dprintf("%p: Made iterator with k = %lx\n", r_, k_);
  if (k_ != ~0ULL) {
    path_[radix_levels] = r_->root_.load();
    if (path_[radix_levels].is_elem())
      leaf_ = radix_levels; // Maybe best to not do this...
    else if (!find_first_leaf(radix_levels - 1))
      k_ = ~0ULL;
  }
  dprintf("%p: Adjusted: k = %lx\n", r_, k_);
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
