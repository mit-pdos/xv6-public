#pragma once

struct padded_length {
  volatile u64 v_ __mpalign__;;
  __padout__;
};

#if defined (XV6_KERNEL)
struct uwq {
  uwq(padded_length *len);
  ~uwq();
  bool  haswork();
  int   trywork();
  void* buffer();

private:
  uwq& operator=(const uwq&);
  uwq(const uwq& x);
  proc* getworker();

  struct spinlock lock_;
  padded_length* len_;

  struct worker {
    int running;
    proc *proc;
  };

  worker worker_[NCPU];
};

int             uwq_trywork(void);
#endif
