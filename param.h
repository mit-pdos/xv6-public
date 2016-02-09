#define NPROC        64  // maximum number of processes
#define KSTACKSIZE 1024  // size of per-process kernel stack
#define NCPU          8  // maximum number of CPUs
#define NOFILE       64  // open files per process
#define NFILE      4096  // open files per system
#define NINODE      100  // maximum number of active i-nodes
#define NDEV         10  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       32  // max exec arguments
#define MAXOPBLOCKS 128  // max # of blocks any FS op writes
#define LOGSIZE      48  // max data blocks in on-disk log
#define NBUF         64  // size of disk block cache
#define FSSIZE    16384  // size of file system in blocks

