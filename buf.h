#ifndef BUF_H_5FB86900_CAAF_11EE_8313_90B11C0C0FF8
#define BUF_H_5FB86900_CAAF_11EE_8313_90B11C0C0FF8
struct buf {
  int flags;
  uint dev;
  uint blockno;
  struct sleeplock lock;
  uint refcnt;
  struct buf *prev; // LRU cache list
  struct buf *next;
  struct buf *qnext; // disk queue
  uchar data[BSIZE];
};
#define B_VALID 0x2  // buffer has been read from disk
#define B_DIRTY 0x4  // buffer needs to be written to disk

#endif
