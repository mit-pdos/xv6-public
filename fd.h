struct fd {
  enum { FD_CLOSED, FD_NONE, FD_PIPE } type;
  int ref; // reference count
  char readable;
  char writeable;
  struct pipe *pipe;
};

extern struct fd fds[NFD];
