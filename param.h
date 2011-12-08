#pragma once
#define NPROC        64  // maximum number of processes
#define KSTACKSIZE 8192  // size of per-process kernel stack
#define NOFILE       16  // open files per process
#define NFILE       100  // open files per system
#define NBUF      10000  // size of disk block cache
#define NINODE     5000  // maximum number of active i-nodes
#define NDEV         10  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       32  // max exec arguments
#define MAXNAME      16  // max string names
#define INF          (~0UL)
#define CACHELINE    64  // cache line size
#define CPUKSTACKS   (NPROC + NCPU)
#define QUANTUN      10  // scheduling time quantum and tick length (in msec)
#define WQSHIFT       4  // 2^WORKSHIFT work queue slots
#define VICTIMAGE 1000000 // cycles a proc executes before an eligible victim
#if defined(HW_josmp)
#define NCPU         16  // maximum number of CPUs
#define MTRACE       0
#define WQENABLE     0   // Enable work queue
#define PERFSIZE     1<<30ull
#elif defined(HW_qemu)
#define NCPU         4   // maximum number of CPUs
#define MTRACE       0
#define WQENABLE     0   // Enable work queue
#define PERFSIZE     16<<20ull
#elif defined(HW_ud0)
#define NCPU         4   // maximum number of CPUs
#define MTRACE       0
#define WQENABLE     0   // Enable work queue
#define PERFSIZE     512<<20ull
#else
#error "Unknown HW"
#endif
