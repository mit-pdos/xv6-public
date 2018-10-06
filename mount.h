struct mount {
  struct mount *parent;
  struct inode *mountpoint;
  int ref;
  uint dev;
};

#define NMOUNT (200)
