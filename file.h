struct file {
  enum { FD_CLOSED, FD_NONE, FD_PIPE, FD_INODE } type;
  int ref; // reference count
  char readable;
  char writable;
  struct pipe *pipe;
  struct uinode *ip;
  uint off;
};
