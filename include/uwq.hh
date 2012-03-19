#pragma once

struct padded_length {
  volatile u64 v_ __mpalign__;;
  __padout__;
};

#if defined (XV6_KERNEL)
struct uwq : public referenced, public rcu_freed {
  static uwq* alloc(vmap* vmap, filetable *ftable);
  bool  haswork();
  int   trywork();
  void* buffer();

  void  setuentry(uptr uentry);

  virtual void do_gc(void) { delete this; }

protected:
  virtual void onzero() const { gc_delayed((uwq*)this); }

private:
  uwq(vmap* vmap, filetable* ftable, padded_length *len);
  ~uwq();
  uwq& operator=(const uwq&);
  uwq(const uwq& x);
  proc* getworker();
  proc* allocworker();
  void  finishworkers();
  NEW_DELETE_OPS(uwq);

  struct spinlock lock_;
  vmap* vmap_;
  filetable* ftable_;
  padded_length* len_;
  uptr uentry_;
  uptr ustack_;

  struct worker {
    bool running;
    proc *proc;
  };
  worker worker_[NCPU];
};

int             uwq_trywork(void);
#endif
