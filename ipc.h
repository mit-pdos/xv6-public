struct ipcctl {
  volatile char done:1;
  volatile char submitted:1;

  off_t off;
  volatile long result;
};
