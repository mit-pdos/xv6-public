struct file {
  enum { FD_CLOSED, FD_NONE, FD_PIPE, FD_FILE } type;
  int ref; // reference count
  char readable;
  char writable;
  struct pipe *pipe;
  struct inode *ip;
  uint off;
};
