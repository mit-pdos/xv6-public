struct spinlock {
  char *name;
  uint locked;
  uint pc;
  int cpu;
};
