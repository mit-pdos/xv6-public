#ifndef XV6_FILE_H
#define XV6_FILE_H

#include "fs.h"
#include "sleeplock.h"
#include "cgfs.h"

struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE, FD_CG } type;
  int ref; // reference count
  char readable;
  char writable;
  uint off;
  union {
    // FD_PIPE
    struct pipe *pipe;

    // FD_INODE
    struct {
      struct inode *ip;
      struct mount *mnt;
    };

    // FD_CG
    struct {
      struct cgroup *cgp;
      char cgfilename[MAX_CGROUP_FILE_NAME_LENGTH];
    };
  };
};


// in-memory copy of an inode
struct inode {
  uint dev;           // Device number
  uint inum;          // Inode number
  int ref;            // Reference count
  struct sleeplock lock; // protects everything below here
  int valid;          // inode has been read from disk?

  short type;         // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT+1];
};

// table mapping major device number to
// device functions
struct devsw {
  int (*read)(struct inode*, char*, int);
  int (*write)(struct inode*, char*, int);
};

extern struct devsw devsw[];

#define CONSOLE 1

#endif
