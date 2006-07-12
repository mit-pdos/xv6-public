struct spinlock {
  unsigned int locked;
  unsigned who;
  int count;
  unsigned locker_pc;
};
