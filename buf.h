struct buf {
  int flags;
  uint dev;
  uint sector;
  int readbusy;
  struct buf *prev; // LRU cache list
  struct buf *next;
  struct buf *qnext; // disk queue
  struct condvar cv;
  struct spinlock lock;
  uchar data[512];
};
#define B_BUSYR 0x1  // buffer is locked by some process
#define B_BUSYW 0x2  // buffer is locked by some process
#define B_VALID 0x4  // buffer has been read from disk
#define B_DIRTY 0x8  // buffer needs to be written to disk

