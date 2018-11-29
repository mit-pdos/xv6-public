// Long-term locks for processes
struct sleeplock {
  uint locked;       // Is the lock held?
  struct spinlock lk; // spinlock protecting this sleep lock
  int lockedstat;    // 0 : no lock 1 : lock write 2 : all lock

  // For debugging:
  char *name;        // Name of lock.
  int pid;           // Process holding lock
};

