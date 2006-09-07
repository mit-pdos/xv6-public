// in-core file system types

struct inode {
  uint dev;           // Device number
  uint inum;          // Inode number
  int ref;            // Reference count
  int busy;           // Is the inode "locked"?

  short type;         // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NADDRS];
};

extern uint rootdev;

#define NAMEI_LOOKUP 1
#define NAMEI_CREATE 2
#define NAMEI_DELETE 3
