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
    radix_node *new_rn = radix_node::create();
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
      if (elem != nullptr)
        elem->decref(1<<bits_per_level);

      // XXX(austin) This happens for nearly 50% of radix_node
      // allocations.  Is the compare exchange actually right?
      if (elem == nullptr)
        // We know the page is still zeroed
        zfree(new_rn);
      else
        // We already did a batch decref above.  We could zero all of
        // the entries and call the destructor (which will scan the
        // node again).  Instead, we skip the whole thing and free
        // directly.
        kfree(new_rn);
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

radix_node*
radix_node::create()
{
  static_assert(sizeof(radix_node) == PGSIZE,
    "radix_node must be exactly one page");
  return (radix_node*)zalloc("radix_node");
}

radix_node::~radix_node()
{
  for (int i = 0; i < (1<<bits_per_level); i++) {
    child[i].load().release();
  }
}

void
radix_node::operator delete(void *p)
{
  kfree(p);
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

void
radix_iterator::prime_path()
{
  dprintf("%p: Made iterator with k = %lx\n", r_, k_);

  // Load the initial path
  level_ = radix_levels;
  const radix_ptr *node = &r_->root_;
  radix_entry entry;
  while ((entry = node->load()).is_node()) {
    level_--;
    path_[level_] = &entry.node()->child[index(k_, level_)];
    node = path_[level_];
  }

  dprintf("%p: Adjusted: k = %lx\n", r_, k_);
}

void
radix_iterator::advance()
{
  while (true) {
    // As long as we haven't reached our limit or an element, advance
    // to the next key.
    k_ += (u64)1 << (level_ * bits_per_level);
    if (k_ == key_limit_)
      return;

    // If we've reached the end of this node, move up the tree until
    // we find a node that has more entries.  (Note that we don't have
    // to worry about going off the top because our key_limit_
    // prevents that.)
    while (index(k_, level_) == 0) {
      level_++;
    }
    assert(level_ <= radix_levels);

    // Update our path pointer at the level we've moved over on
    path_[level_]++;

    // Move down the tree as much as we can
    radix_entry entry;
    while ((entry = path_[level_]->load()).is_node()) {
      path_[level_ - 1] = &entry.node()->child[0];
      level_--;
    }

    // Did we reach a non-null leaf?
    if (!entry.is_null())
      return;
  }
}
