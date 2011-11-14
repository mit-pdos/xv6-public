#pragma once
#define NPROC        64  // maximum number of processes
#define KSTACKSIZE 8192  // size of per-process kernel stack
#define NCPU          4  // maximum number of CPUs
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
#define PHYSTOP      0xE000000 // use phys mem up to here as free pool
#define CPUKSTACKS   (NPROC + NCPU)
#define QUANTUN      10  // scheduling time quantum and tick length (in msec)
#define WQSHIFT      4   // 2^WORKSHIFT work queue slots
#define WQENABLE     1   // Enable work queue
