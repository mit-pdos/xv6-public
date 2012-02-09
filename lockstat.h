#include "queue.h"

#define LOCKSTAT_MAGIC 0xb4cd79c1b2e46f40ull

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
  u64 magic;
  LIST_ENTRY(klockstat) link;
  struct lockstat s;
};

#define LOCKSTAT_START     1
#define LOCKSTAT_STOP      2
#define LOCKSTAT_CLEAR     3

// Debug knobs
#define LOCKSTAT_BIO       1
#define LOCKSTAT_CONDVAR   0
#define LOCKSTAT_CONSOLE   1
#define LOCKSTAT_CRANGE    1
#define LOCKSTAT_FS        1
#define LOCKSTAT_GC        1
#define LOCKSTAT_KALLOC    1
#define LOCKSTAT_KMALLOC   1
#define LOCKSTAT_NET       1
#define LOCKSTAT_PIPE      1
#define LOCKSTAT_PROC      1
#define LOCKSTAT_SCHED     1
#define LOCKSTAT_VM        1
#define LOCKSTAT_WQ        1
