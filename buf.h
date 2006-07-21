struct buf {
  int flags;
  uchar data[512];
};
#define B_BUSY 0x1
