struct condvar {
  char name[MAXNAME];
  struct spinlock lock;
  struct proc *waiters;
  void *chan;                  // If non-zero, sleeping on chan
};
