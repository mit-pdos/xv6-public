#define NPROC        64  // maximum number of processes
#define KSTACKSIZE 4096  // size of per-process kernel stack
#define NCPU          2  // maximum number of CPUs
#define NOFILE       16  // open files per process
#define NFILE       100  // open files per system
#define NBUF      10000  // size of disk block cache
#define NINODE     5000  // maximum number of active i-nodes
#define NDEV         10  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       32  // max exec arguments
#define MAXNAME      16  // max string names
#define MINCYCTHRESH 1000000  // min cycles a proc executes on a core before allowed to be stolen
#define INF          (~0UL)
#define CACHELINE    64  // cache line size
#define MTRACE       0
