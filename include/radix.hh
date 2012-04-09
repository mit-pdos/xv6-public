#pragma once

/*
 * A page-table-like structure for mapping fixed-length keys to void* ptrs.
 */

#include "gc.hh"
#include "markptr.hh"

enum { bits_per_level = 6 };
enum { key_bits = 36 };
enum { radix_levels = (key_bits + bits_per_level - 1) / bits_per_level };

class radix_elem : public rcu_freed {
 private:
  bool deleted_;
  std::atomic<u64> ref_;

 public:
  radix_elem() : rcu_freed("radix_elem"), deleted_(false), ref_(0) {}
  bool deleted() { return deleted_; }
  void decref() { if (--ref_ == 0) { deleted_ = true; gc_delayed(this); } }
  void incref() { ref_++; }
};

struct radix_node {
  markptr<void> ptr[1 << bits_per_level];
  radix_node() {
    for (int i = 0; i < sizeof(ptr) / sizeof(ptr[0]); i++)
      ptr[i] = 0;
  }
  NEW_DELETE_OPS(radix_node)
};

struct radix;

struct radix_range {
  radix* r_;
  u64 start_;
  u64 size_;

  radix_range(radix* r, u64 start, u64 size);
  radix_range(radix_range&&);
  ~radix_range();

  void replace(u64 start, u64 size, radix_elem* val);

  radix_range(const radix_range&) = delete;
  void operator=(const radix_range&) = delete;
};

struct radix {
  markptr<void> root_;
  u32 shift_;

  radix(u32 shift) : root_(0), shift_(shift) {
    root_.ptr() = new radix_node();
  }
  radix_elem* search(u64 key);
  radix_range search_lock(u64 start, u64 size);

  // k is shifted value.
  u64 skip_empty(u64 k) const;

  NEW_DELETE_OPS(radix)
};

struct radix_iterator {
  const radix* r_;
  u64 k_;

  radix_iterator(const radix* r, u64 k) : r_(r), k_(r->skip_empty(k)) {}
  radix_iterator &operator++() { k_++; k_ = r_->skip_empty(k_); return *this; }
  radix_elem* operator*();
  bool operator==(const radix_iterator &other) {
    return r_ == other.r_ && k_ == other.k_; }
  bool operator!=(const radix_iterator &other) {
    return r_ != other.r_ || k_ != other.k_; }
};

struct radix_iterator2 {
  const radix* r_;
  u64 k_;
  // path_[i] is the node at level i. Note that the leaf is at zero
  // and is radix_elem. The rest are radix_node. For now we assume all
  // leaves are at level 0. Later we'll steal a bit for them. The root
  // is path_[radix_levels].
  void *path_[radix_levels+1];

  radix_iterator2(const radix* r, u64 k);

  radix_iterator2 &operator++() {
    if (!advance(radix_levels-1)) k_ = ~0ULL;
    return *this;
  }
  radix_elem* operator*() {
    return (radix_elem*)path_[0];
  }
  radix_node* node(u32 level) { return (radix_node*)path_[level]; }

  // Compare equality on just the key.
  bool operator==(const radix_iterator2 &other) {
    return r_ == other.r_ && k_ == other.k_; }
  bool operator!=(const radix_iterator2 &other) {
    return r_ != other.r_ || k_ != other.k_; }

private:
  bool find_first_leaf(u32 level);
  bool advance(u32 level);
};

#define radix_iterator radix_iterator2

static inline radix_iterator
begin(const radix &r) { return radix_iterator(&r, 0); }

static inline radix_iterator
end(const radix &r) { return radix_iterator(&r, ~0ULL); }
// What we really need is one-past-the-last...

static inline radix_iterator
begin(const radix_range &rr) { return radix_iterator(rr.r_, rr.start_); }

static inline radix_iterator
end(const radix_range &rr) { return radix_iterator(rr.r_, rr.start_ + rr.size_); }

#undef radix_iterator
