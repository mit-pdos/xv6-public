#include "gc.hh"
#include "atomic.hh"
#include "cpputil.hh"

using std::atomic;

struct buf : public rcu_freed {
  atomic<int> flags;
  u32 dev;
  u64 sector;
  struct buf *prev; // LRU cache list
  struct buf *next;
  struct buf *qnext; // disk queue
  char lockname[16];
  struct condvar cv;
  struct spinlock lock;
  u8 data[512];

  buf() : rcu_freed("buf") {}
  virtual void do_gc() { delete this; }
  NEW_DELETE_OPS(buf)
};
#define B_BUSY  0x1  // buffer is locked by some process
#define B_VALID 0x2  // buffer has been read from disk
#define B_DIRTY 0x4  // buffer needs to be written to disk

