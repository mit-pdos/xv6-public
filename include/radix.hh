#pragma once

/*
 * A page-table-like structure for mapping fixed-length keys to void* ptrs.
 */

#include "markptr.hh"

enum { bits_per_level = 9 };
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
  NEW_DELETE_OPS(radix)
};

struct radix_iterator {
  const radix* r_;
  u64 k_;

  radix_iterator(const radix* r, u64 k) : r_(r), k_(k) {}
  radix_iterator &operator++() { k_++; return *this; }
  radix_elem* operator*();
  bool operator==(const radix_iterator &other) {
    return r_ == other.r_ && k_ == other.k_; }
  bool operator!=(const radix_iterator &other) {
    return r_ != other.r_ || k_ != other.k_; }
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

