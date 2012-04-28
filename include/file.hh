#pragma once

#include "cpputil.hh"
#include "ns.hh"
#include "gc.hh"
#include "atomic.hh"
#include "ref.hh"

class dirns;

u64 namehash(const strbuf<DIRSIZ>&);

struct file : public referenced, public rcu_freed {
  static file* alloc();
  file*        dup();
  int          stat(struct stat*);
  int          read(char *addr, int n);
  ssize_t      pread(char *addr, size_t n, off_t offset);
  ssize_t      pwrite(const char *addr, size_t n, off_t offset);
  int          write(const char *addr, int n);

  enum { FD_NONE, FD_PIPE, FD_INODE, FD_SOCKET } type;  

  char readable;
  char writable;

  int socket;
  struct pipe *pipe;
  struct inode *ip;
  u32 off;

  virtual void do_gc(void);

private:
  file();
  file& operator=(const file&);
  file(const file& x);
  NEW_DELETE_OPS(file);

protected:
  virtual void onzero() const;
};

// in-core file system types
struct inode : public referenced, public rcu_freed
{
  static inode* alloc(u32 dev, u32 inum);
  virtual void do_gc() { delete this; }
  void init();

  void link();
  void unlink();
  short nlink();
  
  inode& operator=(const inode&) = delete;
  inode(const inode& x) = delete;
  
  u32 dev;           // Device number
  u32 inum;          // Inode number
  u32 gen;           // Generation number
  int flags;         // I_BUSY, I_VALID
  std::atomic<int> readbusy;
  struct condvar cv;
  struct spinlock lock;
  char lockname[16];
  std::atomic<dirns*> dir;

  short type;        // copy of disk inode
  short major;
  short minor;
  u32 size;
  std::atomic<u32> addrs[NDIRECT+1];
  std::atomic<volatile u32*> iaddrs;

private:
  inode();
  ~inode();
  NEW_DELETE_OPS(inode)

  short nlink_;

protected:
  virtual void onzero() const;
};

#define I_BUSYR 0x1
#define I_BUSYW 0x2
#define I_VALID 0x4
#define I_FREE 0x8


// device implementations

struct devsw {
  int (*read)(struct inode*, char*, u32, u32);
  int (*write)(struct inode*, const char*, u32, u32);
  void (*stat)(struct inode*, struct stat*);
};

extern struct devsw devsw[];
