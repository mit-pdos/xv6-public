struct buf {
  int flags;
  uint dev;
  uint sector;
  struct buf *prev; // LRU cache list
  struct buf *next;
  struct buf *qnext; // disk queue
  int done;
  uchar data[512];
};
#define B_BUSY 0x1  // buffer is locked by some process
#define B_VALID 0x2 // buffer contains the data of the sector
#define B_WRITE 0x4 // asking device driver to write, else read
