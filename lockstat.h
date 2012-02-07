#include "queue.h"

struct cpulockstat {
  u64 acquires;
  u64 contends;
  u64 locking;
  u64 locked;

  u64 locking_ts;
  u64 locked_ts;
  __padout__;
} __mpalign__;

struct lockstat {
  char name[16];
  struct cpulockstat cpu[NCPU] __mpalign__;
};

struct klockstat {
  u8 active;
  LIST_ENTRY(klockstat) link;
  struct lockstat s;
};

#define LOCKSTAT_START 1 
#define LOCKSTAT_STOP  2 
#define LOCKSTAT_CLEAR 3
