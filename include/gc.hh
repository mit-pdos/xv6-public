#pragma once

#define RCU_TYPE_DEBUG  1

class rcu_freed {
 public:
  u64 _rcu_epoch;
  rcu_freed *_rcu_next;
#if RCU_TYPE_DEBUG
  const char *_rcu_type;
#endif

  rcu_freed(const char *debug_type)
#if RCU_TYPE_DEBUG
    : _rcu_type(debug_type)
#endif
  {}

  virtual void do_gc(void) = 0;
} __mpalign__;

void gc_begin_epoch();
void gc_end_epoch();

class scoped_gc_epoch {
 private:
  bool valid;

 public:
  scoped_gc_epoch() { valid = true; gc_begin_epoch(); }
  ~scoped_gc_epoch() { if (valid) gc_end_epoch(); }

  scoped_gc_epoch(const scoped_gc_epoch&) = delete;
  scoped_gc_epoch(scoped_gc_epoch &&other) {
    valid = other.valid;
    other.valid = false;
  }
};

