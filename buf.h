struct buf {
  int flags;
  uint dev;
  uint sector;
  uchar data[512];
};
#define B_BUSY 0x1
#define B_VALID 0x2
