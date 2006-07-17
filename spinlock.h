struct spinlock {
  uint locked;
  uint32_t pc;
  int cpu;
};
