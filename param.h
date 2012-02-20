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
#define NEPOCH        4
#define CACHELINE    64  // cache line size
#define CPUKSTACKS   (NPROC + NCPU)
#define QUANTUM      10  // scheduling time quantum and tick length (in msec)
#define CILKSHIFT     4  // 2^WORKSHIFT work queue slots
#define VICTIMAGE 1000000 // cycles a proc executes before an eligible victim
#define VERBOSE       0  // print kernel diagnostics
#define	SPINLOCK_DEBUG 1 // Debug spin locks
#define LOCKSTAT      1
#define VERIFYFREE    LOCKSTAT
#define ALLOC_MEMSET  1
#define KSHAREDSIZE   (32 << 10)
#define WQSHIFT       4
#if defined(HW_josmp)
#define NCPU         16  // maximum number of CPUs
#define MTRACE       0
#define PERFSIZE     (1<<20ull)
#elif defined(HW_qemu)
#define NCPU         4   // maximum number of CPUs
#define MTRACE       0
#define PERFSIZE     (16<<20ull)
#elif defined(HW_ud0)
#define NCPU         4   // maximum number of CPUs
#define MTRACE       0
#define PERFSIZE     (512<<20ull)
#elif defined(HW_user)
#define NCPU        256
#define MTRACE      0
#define PERFSIZE    (16<<20ull)
#else
#error "Unknown HW"
#endif
