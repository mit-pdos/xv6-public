#include "types.h"

enum uprocstate {U_EMBRYO = 1, U_SLEEPING = 2, U_RUNNABLE = 3, U_RUNNING = 4, U_ZOMBIE = 5};

struct uproc {
  char name[16];               // Process name (debugging)
  int pid;                     // Process ID
  struct proc *parent;         // Parent process
  uint sz;                     // Size of process memory (bytes)
  enum uprocstate state;       // Process state
  int chan;                    // If non-zero, sleeping on chan
  int killed;                  // If non-zero, have been killed
};