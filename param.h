#define NPROC       128  // maximum number of processes
#define KSTACKSIZE 4096  // size of per-process kernel stack
#define NCPU         32  // maximum number of CPUs
#define NOFILE       32  // open files per process
#define NFILE       256  // open files per system
#define NINODE      128  // maximum number of active i-nodes
#define NDEV         10  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       32  // max exec arguments
#define MAXOPBLOCKS  10  // max # of blocks any FS op writes
#define LOGSIZE      30  // max data blocks in on-disk log
#define NBUF        128  // size of disk block cache
#define FSSIZE    16384  // size of file system in blocks

