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
