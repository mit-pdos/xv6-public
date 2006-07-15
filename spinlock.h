struct spinlock {
  unsigned int locked;
  unsigned locker_pc;
};
