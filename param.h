#pragma once
#define DEBUG         1
#define NPROC        64  // maximum number of processes
#define KSTACKSIZE 8192  // size of per-process kernel stack
#define NOFILE       64  // open files per process
#define NFILE       100  // open files per system
#define NBUF      10000  // size of disk block cache
#define NINODE     5000  // maximum number of active i-nodes
#define NDEV         10  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       32  // max exec arguments
#define MAXNAME      16  // max string names
#define NEPOCH        4
#define CACHELINE    64  // cache line size
#define CPUKSTACKS   (NPROC + NCPU*2)
#define QUANTUM      10  // scheduling time quantum and tick length (in msec)
#define CILKSHIFT     4  // 2^WORKSHIFT work queue slots
#define VICTIMAGE 1000000 // cycles a proc executes before an eligible victim
#define VERBOSE       0  // print kernel diagnostics
#define SPINLOCK_DEBUG DEBUG // Debug spin locks
#define RCU_TYPE_DEBUG DEBUG
#define LOCKSTAT      DEBUG
#define VERIFYFREE    0  // Unreliable, e.g. vma's vmnode pointer gets reused
#define ALLOC_MEMSET  DEBUG
#define KSHAREDSIZE   (32 << 10)
#define USERWQSIZE    (1 << 14)
#define USTACKPAGES   4
#define WQSHIFT       7
#define CILKENABLE    0
#if defined(HW_qemu)
#define NCPU         8   // maximum number of CPUs
#define MTRACE       0
#define PERFSIZE     (16<<20ull)
#elif defined(HW_josmp)
#define NCPU         16  // maximum number of CPUs
#define MTRACE       0
#define PERFSIZE     (1<<20ull)
#elif defined(HW_ud0)
#define NCPU         4   // maximum number of CPUs
#define MTRACE       0
#define PERFSIZE     (512<<20ull)
#elif defined(HW_tom)
#define NCPU         48  // maximum number of CPUs
#define MTRACE       0
#define PERFSIZE     (1<<20ull)
#elif defined(HW_user)
#define NCPU        256
#define MTRACE      0
#define PERFSIZE    (16<<20ull)
#elif defined(HW_wq)
#define NCPU        2
#define MTRACE      0
#define PERFSIZE    (16<<20ull)
#else
#error "Unknown HW"
#endif
