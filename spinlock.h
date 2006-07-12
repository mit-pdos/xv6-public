struct spinlock {
  unsigned int locked;
  struct proc *p;
  int count;
  unsigned locker_pc;
};
