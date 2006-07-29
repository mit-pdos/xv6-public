struct fd {
  enum { FD_CLOSED, FD_NONE, FD_PIPE, FD_FILE } type;
  int ref; // reference count
  char readable;
  char writeable;
  struct pipe *pipe;
  struct inode *ip;
};

extern struct fd fds[NFD];
