#pragma once

class rcu_freed {
 private:
  u64 _rcu_epoch;
  rcu_freed *_rcu_next;

 public:
  virtual ~rcu_freed() {}
};

void del_rcu_freed(void*);
