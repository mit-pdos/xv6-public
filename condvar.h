struct condvar {
  struct spinlock lock;
  struct proc *waiters;
};
