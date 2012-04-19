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
  while (cur.state() != entry_node) {
    // If we're locked, just spin and try again.
    if (cur.state() == entry_locked) {
      cur = ptr->load();
      continue;
    }

    // Make a new node.
    assert(cur.state() == entry_unlocked);
    radix_elem *elem = cur.elem();
    // FIXME: This might throw. Might need to unlock the things you've
    // already hit.
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

      delete new_rn;
    }
  }
  return cur;
}

// Runs |cb| of a set of leaves whose disjoint union is the range
// [start, end). Callback returns the last known state of the
// radix_ptr. It is assumed |cb| does not convert the leaf into a
// node. If |cb| returns an entry_node, we recurse into the node and
// call |cb| on the new subtree.
template <class CB>
void
update_range(radix_entry cur, radix_ptr *ptr, CB cb,
             u64 cur_start, u64 cur_end,
             u64 start, u64 end, u32 level = radix_levels)
{
  assert(level_size(level) == cur_end - cur_start);
  // Assert that our ranges intersect; if this fails, we got the loop
  // below wrong.
  assert(cur_start < end && start < cur_end)

  // If our range is not strictly contained in the target, ensure we
  // are at a node.
  if (start > cur_start || end < cur_end) {
    cur = push_down(cur, ptr);
  }

  if (cur.is_elem()) {
    // If we're here, the target range must completely contain this
    // element.
    assert(start <= cur_start && cur_end <= end);
    dprintf(" -> [%lx, %lx); size = %lx\n", cur_start, cur_end, cur_end - cur_start);
    cur = cb(cur, ptr);
  }

  // Recurse if we became a node or we already one.
  if (cur.is_node()) {
    // Find the place to start.
    if (start < cur_start)
      start = cur_start;
    assert(level > 0);
    int i = index(start, level - 1);
    u64 child_size = (cur_end - cur_start) >> bits_per_level;
    u64 child_start = cur_start + i * child_size;
    for (; (i < (1<<bits_per_level)) && (child_start < end);
         i++, child_start += child_size) {
      radix_ptr *child = &cur.node()->child[i];
      update_range(child->load(), child, cb,
                   child_start, child_start + child_size,
                   start, end, level - 1);
    }
  }
}

void
radix_entry::release()
{
  if (is_null()) return;
  if (is_node()) {
    delete node();
  } else {
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

radix_range::radix_range(radix *r, u64 start, u64 size)
  : r_(r), start_(start), size_(size)
{
  u64 end = start_ + size_;
  // Lock the range from left to right.
  dprintf("%p: lock [%lx, %lx)\n", r_, start, start + size);
  update_range(r_->root_.load(), &r_->root_, [](radix_entry cur, radix_ptr *ptr) -> radix_entry {
      while (cur.state() != entry_node) {
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
      // We either managed a lock or someone turned us into a node.
      assert(cur.state() == entry_node || cur.state() == entry_locked);
      return cur;
    }, 0, 1L << key_bits, start_, end);
}

radix_range::~radix_range()
{
  if (!r_)
    return;

  dprintf("%p: unlock [%lx, %lx)\n", r_, start_, start_ + size_);
  update_range(r_->root_.load(), &r_->root_, [](radix_entry cur, radix_ptr *ptr) -> radix_entry {
      do {
        // It had better still be locked.
        assert(cur.state() == entry_locked);
      } while (!ptr->compare_exchange_weak(cur, cur.with_state(entry_unlocked)));
      // Not a node, but let's return what it wants anyway.
      return cur.with_state(entry_unlocked);
    }, 0, 1L << key_bits, start_, start_ + size_);
}

void
radix_range::replace(u64 start, u64 size, radix_elem *val)
{
  start = start >> r_->shift_;
  size = size >> r_->shift_;

  assert(start >= start_);
  assert(start + size <= start_ + size_);

  dprintf("%p: replace: [%lx, %lx) with %p\n", r_, start, start + size, val);
  update_range(r_->root_.load(), &r_->root_, [val](radix_entry cur, radix_ptr *ptr) -> radix_entry {
      do {
        assert(cur.state() == entry_locked);
      } while (!ptr->compare_exchange_weak(cur, radix_entry(val, entry_locked)));
      if (val)
        val->incref();
      // cur is now the old value.
      if (!cur.is_null())
        cur.elem()->decref();
      // Not a node, but let's return what it wants anyway.
      return radix_entry(val, entry_locked);
    }, 0, 1L << key_bits, start, start + size);
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
