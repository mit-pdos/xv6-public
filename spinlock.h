struct spinlock {
  char *name;
  uint locked;
  uint pcs[10];
  int cpu;
};
