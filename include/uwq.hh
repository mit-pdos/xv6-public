#pragma once

struct padded_length {
  volatile u64 v_ __mpalign__;
  __padout__;
};

// Compile-time max number of workers
#define NWORKERS (NCPU-1)
struct uwq_ipcbuf {
  // Run-time max number of workers
  u64 maxworkers __mpalign__;
  padded_length len[NWORKERS]__mpalign__;
};

#if defined (XV6_KERNEL)
bool uwq_trywork(void);

struct uwq;

struct uwq_worker {
  uwq_worker(uwq*, proc*);
  long wait();
  void exit();

  uwq* uwq_;
  proc *proc_;
  bool running_;
  struct spinlock lock_;
  struct condvar cv_;

  NEW_DELETE_OPS(uwq_worker);
};

struct uwq : public referenced, public rcu_freed {
  friend struct uwq_worker;

  static uwq* alloc(vmap* vmap, filetable *ftable);
  bool  haswork() const;
  bool  tryworker();

  void  setuentry(uptr uentry);

  virtual void do_gc(void) { delete this; }

protected:
  virtual void onzero() const;

private:
  uwq(vmap* vmap, filetable* ftable, uwq_ipcbuf *ipc);
  ~uwq();
  uwq& operator=(const uwq&);
  uwq(const uwq& x);
  proc* allocworker();
  void  finish();
  NEW_DELETE_OPS(uwq);

  struct spinlock lock_;
  vmap* vmap_;
  filetable* ftable_;
  uwq_ipcbuf* ipc_;
  uptr uentry_;
  std::atomic<uptr> ustack_;
  std::atomic<u64> uref_;

  std::atomic<uwq_worker*> worker_[NWORKERS];
};
#endif
