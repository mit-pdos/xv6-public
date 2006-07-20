struct spinlock {
  uint locked;
  uint pc;
  int cpu;
};
