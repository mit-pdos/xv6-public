#pragma once

#include "cpputil.hh"
#include "ns.hh"
#include "gc.hh"
#include "atomic.hh"
#include "ref.hh"

u64 namehash(const strbuf<DIRSIZ>&);

struct file : public referenced {
  static file *alloc();
  file *dup();
  int stat(struct stat*);
  int read(char *addr, int n);
  ssize_t pread(char *addr, size_t n, off_t offset);
  int write(char *addr, int n);

  enum { FD_NONE, FD_PIPE, FD_INODE, FD_SOCKET } type;  

  char readable;
  char writable;

  int socket;
  struct pipe *pipe;
  struct inode *ip;
  u32 off;
  NEW_DELETE_OPS(file);

private:
  file();  

protected:
  virtual void onzero() const;
};

// in-core file system types

struct inode : public rcu_freed {
  u32 dev;           // Device number
  u32 inum;          // Inode number
  u32 gen;           // Generation number
  std::atomic<int> ref; // Reference count
  int flags;         // I_BUSY, I_VALID
  std::atomic<int> readbusy;
  struct condvar cv;
  struct spinlock lock;
  char lockname[16];
  std::atomic<xns<strbuf<DIRSIZ>, u32, namehash>*> dir;

  short type;         // copy of disk inode
  short major;
  short minor;
  short nlink;
  u32 size;
  u32 addrs[NDIRECT+1];

  inode();
  ~inode();
  virtual void do_gc() { delete this; }
  NEW_DELETE_OPS(inode)
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
