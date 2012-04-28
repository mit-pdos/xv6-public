#pragma once

/*
 * A page-table-like structure for mapping fixed-length keys to void* ptrs.
 */

#include "gc.hh"

#include <iterator>

enum { bits_per_level = 9 };
enum { key_bits = 36 };
enum { radix_levels = (key_bits + bits_per_level - 1) / bits_per_level };

class radix_elem;
class radix_node;

/*
 * Each pointer to a radix_elem or radix_node can be in one of four
 * states:
 *
 * - pointer to radix_node
 * - unlocked leaf
 * - locked leaf
 * - dead leaf
 *
 * A leaf is either a pointer to a radix_elem or null.
 *
 * Before making semantic modifications to a range, the range must be
 * locked. This is done by locking the leaf pointers (be they to
 * radix_entry or null) corresponding to that range. If necessary, a
 * leaf may be "pushed down" and replaced with a pointer to radix_node
 * full of the old value to get the endpoints accurate. Locking NEVER
 * happens higher level than the current set of leaves.
 *
 * We assuming that a thread attempting to push down a leaf is doing
 * so to lock it.
 *
 * When replacing a range, we'd like to possibly retire old
 * radix_nodes when their contents are all set to be the same. Before
 * doing this, all leaves under that radix_node must be locked. We
 * transition them to 'dead leaf' state. This informs all others
 * attempting to lock the pointer to retry. The radix_node itself is
 * RCU-freed. To avoid restarting writers, set the leaves to the right
 * value too. Replaced elements are written in locked state, to be
 * unlocked when the radix_range goes away.
 *
 * Once a pointer is dead, it stays dead until the containing
 * radix_node is deallocated. Dead pointers do not own references.
 *
 * For now we do not implement the dead state. It is only necessary
 * when collapsing an already-expanded node. It's unclear this
 * optimization is very useful as it requires RCU-freeing radix_nodes,
 * which makes them just over a power of 2 and inefficient to
 * allocate.
 *
 * Races:
 *
 * - If a leaf to be locked (or pushed down) gets pushed down, lock
 *   the new radix_node at a more granular level.
 *
 * - If a leaf to be locked (or pushed down) goes dead, restart
 *   everything from the root. Many values may have gone invalid.
 *
 * - If a leaf to be locked (or pushed down) gets locked, spin.
 *
 * [*] XXX: Try not to bounce on the radix_elem refcount too much.
 */

enum entry_state {
  entry_unlocked = 0,
  entry_locked = 1,
//  entry_dead = 2,
  entry_node = 2,

  entry_mask = 3
};

class radix_entry {
public:
  radix_entry()
    : value_(0 | entry_unlocked) { }
  explicit radix_entry(uptr value)
    : value_(value) { }
  explicit radix_entry(radix_node *ptr)
    : value_(reinterpret_cast<uptr>(ptr) | entry_node) {
    // XXX: This is kinda wonky. Maybe switch the status to
    // entry_unlocked is ptr is null, make null pass both is_elem()
    // and is_node().
    assert(ptr != nullptr);
  }
  explicit radix_entry(radix_elem *ptr, entry_state state = entry_unlocked)
    : value_(reinterpret_cast<uptr>(ptr) | state) {
    assert(state != entry_node);
  }
  explicit radix_entry(decltype(nullptr) nullp,
                       entry_state state = entry_unlocked)
    : value_(0 | state) {
    assert(state != entry_node);
  }

  uptr value() const { return value_; }
  uptr& value() { return value_; }
  entry_state state() const {
    return static_cast<entry_state>(value_ & entry_mask);
  }
  uptr ptr() const { return value_ & ~entry_mask; }

  bool is_node() const { return state() == entry_node; }
  bool is_elem() const { return !is_node(); }
  bool is_null() const { return ptr() == 0; }

  // Convenience function
  radix_entry with_state(entry_state state) {
    return radix_entry(elem(), state);
  }

  radix_elem *elem() const {
    assert(is_elem());
    return reinterpret_cast<radix_elem*>(ptr());
  }
  radix_node *node() const {
    assert(is_node());
    return reinterpret_cast<radix_node*>(ptr());
  }

  void release();
private:
  uptr value_;
};

// Our version of std::atomic doesn't work for structs, even if they
// are integer sized.
class radix_ptr {
public:
  radix_ptr() : ptr_(radix_entry().value()) { }
  radix_ptr(radix_entry e) : ptr_(e.value()) { }
  radix_entry load() const { return radix_entry(ptr_.load()); }
  void store(radix_entry e) { ptr_.store(e.value()); }
  bool compare_exchange_weak(radix_entry &old, radix_entry val) {
    return ptr_.compare_exchange_weak(old.value(), val.value());
  }
private:
  static_assert(sizeof(uptr) == sizeof(radix_entry),
                "radix_entry is a uptr");
  std::atomic<uptr> ptr_;
};

class radix_elem : public rcu_freed {
 public:
  radix_elem() : rcu_freed("radix_elem") {}
  virtual ~radix_elem() {}
  virtual void decref(u64 delta = 1) = 0;
  virtual void incref(u64 delta = 1) = 0;
};

struct radix_node {
  radix_ptr child[1 << bits_per_level];
  // We need to customize not only allocation but initialization, so
  // radix_node has no constructors.  Instead, use create.
  radix_node() = delete;
  radix_node(const radix_node &o) = delete;
  static radix_node *create();
  ~radix_node();

  // Since we use custom allocation for radix_node's, we must also
  // custom delete them.  Note that callers may alternatively use
  // zfree when freeing a radix_node that's known to be empty (for
  // example, after failed optimistic concurrency).
  static void operator delete(void *p);
};

// Assert we have enough spare bits for all flags.
static_assert(alignof(radix_node) > entry_mask,
              "radix_node sufficiently aligned");
static_assert(alignof(radix_elem) > entry_mask,
              "radix_elem sufficiently aligned");

struct radix;
struct radix_iterator;

struct radix_range {
  radix* r_;
  u64 start_;
  u64 size_;

  radix_range(radix* r, u64 start, u64 size);
  radix_range(radix_range&&);
  ~radix_range();

  void replace(u64 start, u64 size, radix_elem* val);

  radix_iterator begin() const;
  radix_iterator end() const;

  radix_range(const radix_range&) = delete;
  void operator=(const radix_range&) = delete;
};

struct radix {
  typedef radix_iterator iterator;

  radix_ptr root_;
  u32 shift_;

  radix(u32 shift) : root_(radix_entry(radix_node::create())), shift_(shift) {
  }
  ~radix();
  radix_elem* search(u64 key);
  radix_range search_lock(u64 start, u64 size);

  iterator find(u64 key);

  radix_iterator begin() const;
  radix_iterator end() const;

  NEW_DELETE_OPS(radix)
};

struct radix_iterator {
  radix_iterator(const radix* r, u64 k, u64 limit, bool skip_nulls)
    : r_(r), k_(k), key_limit_(limit)
  {
    if (k_ != key_limit_)
      prime_path(skip_nulls);
  }
  radix_iterator() = default;
  radix_iterator(const radix_iterator &o) = default;
  radix_iterator(radix_iterator &&o) = default;

  // Move to the next non-null entry in the collection, or end.
  radix_iterator &operator++() {
    assert(k_ < key_limit_);
    advance();
    return *this;
  }

  radix_elem* operator*() const {
    // XXX(austin) If we don't hold a lock on this element, it's
    // possible it will morph into a node under us.
    return path_[level_]->load().elem();
  }

  // Advance the iterator until it points at a non-null entry or end.
  // If the current element is non-null, does nothing.
  radix_iterator &skip_nulls()
  {
    if (!**this)
      ++(*this);
    return *this;
  }

  // Return the key of the iterator's current element.
  u64 key() const
  {
    return k_ << r_->shift_;
  }

  // Return the span of the key space occupied by the iterator's
  // current element.
  u64 span() const
  {
    return (u64)1 << (bits_per_level * level_ + r_->shift_);
  }

  // Return an iterator that points to the next element that is not
  // equal to the current element.  If no such element exists, returns
  // end.  Note that this element may be null.
  radix_iterator next_change() const;

  // Compare equality on just the key.
  bool operator==(const radix_iterator &other) {
    return r_ == other.r_ && k_ == other.k_; }
  bool operator!=(const radix_iterator &other) {
    return r_ != other.r_ || k_ != other.k_; }

private:
  const radix* r_;
  u64 k_;
  // The key value just past the end of the range being iterator over.
  u64 key_limit_;
  // path_[i] points into the child array of the node at level i.
  const radix_ptr *path_[radix_levels];
  // The level of the current element.
  u32 level_;

  // Prepare the initial path_ and level_ based on k_.
  void prime_path(bool skip_nulls);
  // Advance to the next leaf.  If skip_nulls is true, advances to the
  // next non-null leaf.  This assumes that k_ < key_limit_.
  void advance(bool skip_nulls = true);
};

inline radix_iterator
radix_range::begin() const {
  return radix_iterator(r_, start_, start_ + size_, true);
}

inline radix_iterator
radix_range::end() const {
  return radix_iterator(r_, start_ + size_, start_ + size_, true);
}

inline radix_iterator
radix::begin() const {
  return radix_iterator(this, 0, (u64)1 << key_bits, true);
}

inline radix_iterator
radix::end() const {
  return radix_iterator(this, (u64)1 << key_bits, (u64)1 << key_bits, true);
}
