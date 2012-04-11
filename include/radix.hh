#pragma once

/*
 * A page-table-like structure for mapping fixed-length keys to void* ptrs.
 */

#include "gc.hh"

enum { bits_per_level = 6 };
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
  entry_dead = 2,
  entry_node = 3,

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
 private:
  bool deleted_;
  std::atomic<u64> ref_;

 public:
  radix_elem() : rcu_freed("radix_elem"), deleted_(false), ref_(0) {}
  bool deleted() { return deleted_; }
  void decref(u64 delta = 1) {
    if ((ref_ -= delta) == 0) {
      deleted_ = true;
      gc_delayed(this);
    }
  }
  void incref(u64 delta = 1) { ref_ += delta; }
};

struct radix_node : public rcu_freed {
  radix_ptr child[1 << bits_per_level];
  radix_node() : rcu_freed("radix_node") {
  }
  ~radix_node();

  virtual void do_gc() { delete this; }

  NEW_DELETE_OPS(radix_node)
};

// Assert we have enough spare bits for all flags.
static_assert(alignof(radix_node) > entry_mask,
              "radix_node sufficiently aligned");
static_assert(alignof(radix_elem) > entry_mask,
              "radix_elem sufficiently aligned");

struct radix;

struct radix_range {
  radix* r_;
  u64 start_;
  u64 size_;

  scoped_gc_epoch gc_;

  radix_range(radix* r, u64 start, u64 size);
  radix_range(radix_range&&);
  ~radix_range();

  void replace(u64 start, u64 size, radix_elem* val);

  radix_range(const radix_range&) = delete;
  void operator=(const radix_range&) = delete;
};

struct radix {
  radix_ptr root_;
  u32 shift_;

  radix(u32 shift) : root_(radix_entry(new radix_node())), shift_(shift) {
  }
  ~radix();
  radix_elem* search(u64 key);
  radix_range search_lock(u64 start, u64 size);

  NEW_DELETE_OPS(radix)
};

struct radix_iterator {
  const radix* r_;
  u64 k_;
  // path_[i] is the node at level i. Note that the leaf is at zero
  // and is radix_elem. The rest are radix_node. For now we assume all
  // leaves are at level 0. Later we'll steal a bit for them. The root
  // is path_[radix_levels].
  radix_entry path_[radix_levels+1];
  u32 leaf_;

  radix_iterator(const radix* r, u64 k);

  radix_iterator &operator++() {
    if (!advance(radix_levels-1)) k_ = ~0ULL;
    return *this;
  }
  radix_elem* operator*() {
    return path_[leaf_].elem();
  }
  radix_node* node(u32 level) { return path_[level].node(); }

  // Compare equality on just the key.
  bool operator==(const radix_iterator &other) {
    return r_ == other.r_ && k_ == other.k_; }
  bool operator!=(const radix_iterator &other) {
    return r_ != other.r_ || k_ != other.k_; }

private:
  bool find_first_leaf(u32 level);
  bool advance(u32 level);
};

static inline radix_iterator
begin(const radix &r) { return radix_iterator(&r, 0); }

static inline radix_iterator
end(const radix &r) { return radix_iterator(&r, ~0ULL); }
// What we really need is one-past-the-last...

static inline radix_iterator
begin(const radix_range &rr) { return radix_iterator(rr.r_, rr.start_); }

static inline radix_iterator
end(const radix_range &rr) { return radix_iterator(rr.r_, rr.start_ + rr.size_); }
