struct spinlock {
  uint locked;
  uint locker_pc;
};
