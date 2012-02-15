#include "cpputil.hh"
#include "ns.hh"
#include "gc.hh"
#include "atomic.hh"

u64 namehash(const strbuf<DIRSIZ>&);

struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE, FD_SOCKET } type;
  atomic<int> ref; // reference count
  char readable;
  char writable;

  int socket;
  struct pipe *pipe;
  struct inode *ip;
  u32 off;
};


// in-core file system types

struct inode : public rcu_freed {
  u32 dev;           // Device number
  u32 inum;          // Inode number
  u32 gen;           // Generation number
  int ref;           // Reference count
  int flags;         // I_BUSY, I_VALID
  int readbusy;
  struct condvar cv;
  struct spinlock lock;
  char lockname[16];
  xns<strbuf<DIRSIZ>, u32, namehash> *dir;

  short type;         // copy of disk inode
  short major;
  short minor;
  short nlink;
  u32 size;
  u32 addrs[NDIRECT+1];

  inode();
  ~inode();
  virtual void do_gc() { delete this; }
};

#define I_BUSYR 0x1
#define I_BUSYW 0x2
#define I_VALID 0x4
#define I_FREE 0x8


// device implementations

struct devsw {
  int (*read)(struct inode*, char*, u32, u32);
  int (*write)(struct inode*, char*, u32, u32);
  void (*stat)(struct inode*, struct stat*);
};

extern struct devsw devsw[];

#define CONSOLE  1
#define NETIF    2
#define SAMPLER  3
#define DEVLOCKSTAT 4
