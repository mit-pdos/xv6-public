#pragma once

class rcu_freed {
 public:
  u64 _rcu_epoch;
  rcu_freed *_rcu_next;

  virtual ~rcu_freed() {}
} __mpalign__;
