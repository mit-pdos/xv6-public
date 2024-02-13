#ifndef SLEEPLOCK_H_5FB5EE8C_CAAF_11EE_B022_90B11C0C0FF8
#define SLEEPLOCK_H_5FB5EE8C_CAAF_11EE_B022_90B11C0C0FF8
// Long-term locks for processes
struct sleeplock {
  uint locked;       // Is the lock held?
  struct spinlock lk; // spinlock protecting this sleep lock
  
  // For debugging:
  char *name;        // Name of lock.
  int pid;           // Process holding lock
};

#endif
