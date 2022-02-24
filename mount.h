struct mount {
  struct mount *parent;
  struct vfs_inode *mountpoint;
  int ref;
  uint dev;
};

#define NMOUNT (200)
