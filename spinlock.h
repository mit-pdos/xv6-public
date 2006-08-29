struct spinlock {
  char *name;
  uint locked;
  int cpu;
  uint pcs[10];
};
