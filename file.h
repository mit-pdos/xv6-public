#ifndef XV6_FILE_H
#define XV6_FILE_H

#include "fs.h"
#include "sleeplock.h"
#include "cgfs.h"
#include "param.h"

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
      union {
        // cpu
        union {
          struct {
            char active;
            int usage_usec;
            int user_usec;
            int system_usec;
            int nr_periods;
            int nr_throttled;
            int throttled_usec;
          } stat;
          struct {
            int weight;
          } weight;
          struct {
            int max;
            int period;
          } max;
        } cpu;
        // pid
        union {
          struct {
            char active;
            int max;
          } max;
        } pid;
        // cpu_set
        union {
          struct {
            char active;
            int cpu_id;
          } set;
        } cpu_s;
        // freezer
        union {
          struct {
            int frozen;
          } freezer;
        } frz;
        // memory
        union {
          struct {
            char active;
            unsigned int max;
          } max;
        } mem;
      };
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
  int flags;
};

extern struct devsw devsw[];

#define CONSOLE 1

#endif
