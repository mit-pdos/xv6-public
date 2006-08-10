struct spinlock {
  uint magic;
  char *name;
  uint locked;
  int cpu;
  uint pcs[10];
};
