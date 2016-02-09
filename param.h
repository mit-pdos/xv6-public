#define NPROC       128  // maximum number of processes
#define KSTACKSIZE 8192  // size of per-process kernel stack
#define NCPU         16  // maximum number of CPUs
#define NOFILE       64  // open files per process
#define NFILE      4096  // open files per system
#define NINODE      128  // maximum number of active i-nodes
#define NDEV         24  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       64  // max exec arguments
#define MAXOPBLOCKS 128  // max # of blocks any FS op writes
#define LOGSIZE      (MAXOPBLOCKS*3)  // max data blocks in on-disk log
#define NBUF         (MAXOPBLOCKS*3)  // size of disk block cache
#define FSSIZE    16384  // size of file system in blocks

