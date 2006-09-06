#define NPROC        64  // maximum number of processes
#define PAGE       4096  // granularity of user-space memory allocation
#define KSTACKSIZE PAGE  // size of per-process kernel stack
#define NCPU          8  // maximum number of CPUs
#define NOFILE       16  // open files per process
#define NFILE       100  // open files per system
#define NREQUEST    100  // outstanding disk requests
#define NBUF         10  // size of disk block cache
#define NINODE      100  // maximum number of active i-nodes
#define NDEV         10  // maximum major device number
