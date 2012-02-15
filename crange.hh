#pragma once

#include "atomic.hh"

using std::atomic;

struct crange;

struct range : public rcu_freed {
public:
  u64 key;
  u64 size;
  void *value;
  atomic<int> curlevel;  // the current levels it appears on
  int nlevel;            // the number of levels this range should appear
  crange *cr;            // the crange this range is part of
  struct range** next;   // one next pointer per level
  struct spinlock *lock; // on separate cache line?
  range(crange *cr, u64 k, u64 sz, void *v, struct range *n, int nlevel);
  ~range();
  virtual void do_gc() {
    delete this;
  }
  void print(int l);
  void free_delayed();
  void dec_ref(void);
  int lockif(range *e);
} __mpalign__;

struct crange {
private:
  range *crange_head;    // a crange skip list starts with a sentinel range (key 0, sz 0)
public:
  int nlevel;                  // number of levels in the crange skip list
  crange(int nlevel);
  ~crange(void);
  void del(u64 k, u64 sz);
  void add(u64 k, u64 sz, void *v);
  range* search(u64 k, u64 sz, int mod);
  int foreach(int (*cb)(range *r, void *), void *st);
  void print(int);
  void check(struct range *absent);
  int del_index(range *p0, range **e, int l);
  void add_index(int l, range *e, range *p1, range *s1);
  int lock_range(u64 k, u64 sz, int l, range **er, range **pr, range **fr, range **lr, range **sr);
  int find_and_lock(u64 k, u64 sz, range **p0, range **f0, range **l0, range **s0);
};

