// File system implementation.  Four layers:
//   + Blocks: allocator for raw disk blocks.
//   + Files: inode allocator, reading, writing, metadata.
//   + Directories: inode with special contents (list of other inodes!)
//   + Names: paths like /usr/rtm/xv6/fs.c for convenient naming.
//
// Disk layout is: superblock, inodes, block in-use bitmap, data blocks.
//
// This file contains the low-level file system manipulation 
// routines.  The (higher-level) system call implementations
// are in sysfile.c.

/*
 * inode cache will be RCU-managed:
 * 
 * - to evict, mark inode as a victim
 * - lookups that encounter a victim inode must return an error (-E_RETRY)
 * - E_RETRY rolls back to the beginning of syscall/pagefault and retries
 * - out-of-memory error should be treated like -E_RETRY
 * - once an inode is marked as victim, it can be gc_delayed()
 * - the do_gc() method should remove inode from the namespace & free it
 * 
 * - inodes have a refcount that lasts beyond a GC epoch
 * - to bump refcount, first bump, then check victim flag
 * - if victim flag is set, reduce the refcount and -E_RETRY
 *
 */

#include "types.h"
#include "stat.h"
#include "mmu.h"
#include "kernel.hh"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.hh"
#include "fs.h"
#include "buf.hh"
#include "file.hh"
#include "cpu.hh"
#include "kmtrace.hh"
#include "dirns.hh"

#define min(a, b) ((a) < (b) ? (a) : (b))
static void itrunc(struct inode*);
static inode* the_root;

#define IADDRSSZ (sizeof(u32)*NINDIRECT)

// Read the super block.
static void
readsb(int dev, struct superblock *sb)
{
  struct buf *bp;
  
  bp = bread(dev, 1, 0);
  memmove(sb, bp->data, sizeof(*sb));
  brelse(bp, 0);
}

// Zero a block.
static void
bzero(int dev, int bno)
{
  struct buf *bp;
  
  bp = bread(dev, bno, 1);
  memset(bp->data, 0, BSIZE);
  bwrite(bp);
  brelse(bp, 1);
}

// Blocks. 

// Allocate a disk block.
static u32
balloc(u32 dev)
{
  int b, bi, m;
  struct buf *bp;
  struct superblock sb;

  bp = 0;
  readsb(dev, &sb);
  for(b = 0; b < sb.size; b += BPB){
    bp = bread(dev, BBLOCK(b, sb.ninodes), 1);
    for(bi = 0; bi < BPB && bi < (sb.size - b); bi++){
      m = 1 << (bi % 8);
      if((bp->data[bi/8] & m) == 0){  // Is block free?
        bp->data[bi/8] |= m;  // Mark block in use on disk.
        bwrite(bp);
        brelse(bp, 1);
        return b + bi;
      }
    }
    brelse(bp, 1);
  }
  panic("balloc: out of blocks");
}

// Free a disk block.
static void
bfree(int dev, u64 x)
{
  struct buf *bp;
  struct superblock sb;
  int bi, m;
  u32 b = x;

  bzero(dev, b);

  readsb(dev, &sb);
  bp = bread(dev, BBLOCK(b, sb.ninodes), 1);
  bi = b % BPB;
  m = 1 << (bi % 8);
  if((bp->data[bi/8] & m) == 0)
    panic("freeing free block");
  bp->data[bi/8] &= ~m;  // Mark block free on disk.
  bwrite(bp);
  brelse(bp, 1);
}

// Inodes.
//
// An inode is a single, unnamed file in the file system.
// The inode disk structure holds metadata (the type, device numbers,
// and data size) along with a list of blocks where the associated
// data can be found.
//
// The inodes are laid out sequentially on disk immediately after
// the superblock.  The kernel keeps a cache of the in-use
// on-disk structures to provide a place for synchronizing access
// to inodes shared between multiple processes.
// 
// ip->ref counts the number of pointer references to this cached
// inode; references are typically kept in struct file and in proc->cwd.
// When ip->ref falls to zero, the inode is no longer cached.
// It is an error to use an inode without holding a reference to it.
//
// Processes are only allowed to read and write inode
// metadata and contents when holding the inode's lock,
// represented by the I_BUSY flag in the in-memory copy.
// Because inode locks are held during disk accesses, 
// they are implemented using a flag rather than with
// spin locks.  Callers are responsible for locking
// inodes before passing them to routines in this file; leaving
// this responsibility with the caller makes it possible for them
// to create arbitrarily-sized atomic operations.
//
// To give maximum control over locking to the callers, 
// the routines in this file that return inode pointers 
// return pointers to *unlocked* inodes.  It is the callers'
// responsibility to lock them before using them.  A non-zero
// ip->ref keeps these unlocked inodes in the cache.

u64
ino_hash(const pair<u32, u32> &p)
{
  return p.first ^ p.second;
}

static nstbl<pair<u32, u32>, inode*, ino_hash> *ins;

void
initinode(void)
{
  ins = new nstbl<pair<u32, u32>, inode*, ino_hash>();
  the_root = inode::alloc(ROOTDEV, ROOTINO);
  if (ins->insert(make_pair(the_root->dev, the_root->inum), the_root) < 0)
    panic("initinode: insert the_root failed");
  the_root->init();

  if (VERBOSE) {
    struct superblock sb;
    u64 blocks;

    readsb(ROOTDEV, &sb);
    blocks = sb.ninodes/IPB;
    cprintf("initinode: %lu inode blocks (%lu / core)\n",
            blocks, blocks/NCPU);
  }

}

//PAGEBREAK!
// Allocate a new inode with the given type on device dev.
// Returns a locked inode.
struct inode*
ialloc(u32 dev, short type)
{
  struct buf *bp;
  struct dinode *dip;
  struct superblock sb;

  readsb(dev, &sb);
  for (int k = myid()*IPB; k < sb.ninodes; k += (NCPU*IPB)) {
    for(int inum = k; inum < k+IPB && inum < sb.ninodes; inum++){
      if (inum == 0)
        continue;
      bp = bread(dev, IBLOCK(inum), 0);
      dip = (struct dinode*)bp->data + inum%IPB;
      int seemsfree = (dip->type == 0);
      brelse(bp, 0);
      if(seemsfree){
        // maybe this inode is free. look at it via the
        // inode cache to make sure.
        struct inode *ip = iget(dev, inum);
        assert(ip->valid());
        ilock(ip, 1);
        if(ip->type == 0){
          ip->type = type;
          ip->gen += 1;
          if(ip->nlink() || ip->size || ip->addrs[0])
            panic("ialloc not zeroed");
          iupdate(ip);
          return ip;
        }
        iunlockput(ip);
        //cprintf("ialloc oops %d\n", inum); // XXX harmless
      }
    }
  }
  cprintf("ialloc: 0/%u inodes\n", sb.ninodes);
  return nullptr;
}

// Copy inode, which has changed, from memory to disk.
void
iupdate(struct inode *ip)
{
  struct buf *bp;
  struct dinode *dip;

  bp = bread(ip->dev, IBLOCK(ip->inum), 1);
  dip = (struct dinode*)bp->data + ip->inum%IPB;
  dip->type = ip->type;
  dip->major = ip->major;
  dip->minor = ip->minor;
  dip->nlink = ip->nlink();
  dip->size = ip->size;
  dip->gen = ip->gen;
  memmove(dip->addrs, ip->addrs, sizeof(ip->addrs));
  bwrite(bp);
  brelse(bp, 1);

  if (ip->addrs[NDIRECT] != 0) {
    assert(ip->iaddrs.load() != nullptr);
    bp = bread(ip->dev, ip->addrs[NDIRECT], 1);
    memmove(bp->data, (void*)ip->iaddrs.load(), IADDRSSZ);
    bwrite(bp);
    brelse(bp, 1);
  }
}

// Find the inode with number inum on device dev
// and return the in-memory copy.
// The inode is not locked, so someone else might
// be modifying it.
// But it has a ref count, so it won't be freed or reused.
// Though unlocked, all fields will be present,
// so looking a ip->inum and ip->gen are OK even w/o lock.
inode::inode()
  : rcu_freed("inode"),
    flags(0), readbusy(0)
{
  dir.store(nullptr);
  iaddrs.store(nullptr);
}

inode::~inode()
{
  auto d = dir.load();
  if (d) {
    d->remove(strbuf<DIRSIZ>("."));
    d->remove(strbuf<DIRSIZ>(".."));
    gc_delayed(d);
    assert(cmpxch(&dir, d, (decltype(d)) 0));
  }
  if (iaddrs.load() != nullptr) {
    kmfree((void*)iaddrs.load(), IADDRSSZ);
    iaddrs.store(nullptr);
  }

  destroylock(&lock);
  destroycondvar(&cv);
}

static inode*
__iget(u32 dev, u32 inum, bool* haveref)
{
  inode* ip;

  // Assumes caller is holding a gc_epoch

 retry:
  // Try for cached inode.
  ip = ins->lookup(make_pair(dev, inum));
  if (ip) {
    if (!(ip->flags & I_VALID)) {
      acquire(&ip->lock);
      while((ip->flags & I_VALID) == 0)
        cv_sleep(&ip->cv, &ip->lock);
      release(&ip->lock);
    }
    *haveref = false;
    return ip;
  }
  
  // Allocate fresh inode cache slot.
  ip = inode::alloc(dev, inum);
  if (ip == nullptr)
    panic("igetnoref: should throw_bad_alloc()");
  
  // Lock the inode
  ip->flags = I_BUSYR | I_BUSYW;
  ip->readbusy = 1;

  if (ins->insert(make_pair(ip->dev, ip->inum), ip) < 0) {
    // We haven't touched anything on disk, so we can
    // gc_delayed, instead of ip->onzero() (via ip->dec())
    gc_delayed(ip);
    goto retry;
  }
  ip->init();

  iunlock(ip);
  *haveref = true;
  return ip;
}

inode*
iget(u32 dev, u32 inum)
{
  bool haveref;
  inode* ip;

retry:
  {
    scoped_gc_epoch e;
    ip = __iget(dev, inum, &haveref);
    if (!haveref)
      if (!ip->tryinc())
        goto retry;
  }
  return ip;
}

inode*
inode::alloc(u32 dev, u32 inum)
{
  inode* ip = new inode();
  if (ip == nullptr)
    return nullptr;
  ip->dev = dev;
  ip->inum = inum;
  snprintf(ip->lockname, sizeof(ip->lockname), "cv:ino:%d", ip->inum);
  initlock(&ip->lock, ip->lockname+3, LOCKSTAT_FS);
  initcondvar(&ip->cv, ip->lockname);
  
  return ip;
}

void
inode::init(void)
{
  struct buf *bp = bread(dev, IBLOCK(inum), 0);
  struct dinode *dip = (struct dinode*)bp->data + inum%IPB;
  type = dip->type;
  major = dip->major;
  minor = dip->minor;
  nlink_ = dip->nlink;
  if (nlink_ > 0)
    idup(this);
  size = dip->size;
  gen = dip->gen;
  memmove(addrs, dip->addrs, sizeof(addrs));
  brelse(bp, 0);
  flags |= I_VALID;
}

void
inode::link(void)
{
  // Must hold ilock if inode is accessible by multiple threads
  if (++nlink_ == 1) {
    // A non-zero nlink_ holds a reference to the inode
    idup(this);
  }
}

void
inode::unlink(void)
{
  // Must hold ilock if inode is accessible by multiple threads
  if (--nlink_ == 0) {
    // This should never be the last reference..
    iput(this);
  }
}

short
inode::nlink(void)
{
  // Must hold ilock if inode is accessible by multiple threads
  return nlink_;
}

void
inode::onzero(void) const
{
  inode* ip = (inode*)this;

  acquire(&ip->lock);
  if (ip->nlink())
      panic("iput: nlink %u\n", ip->nlink());

  // inode is no longer used: truncate and free inode.
  if(ip->flags & (I_BUSYR | I_BUSYW)) {
    // race with iget
    panic("iput busy");
  }
  if(ip->flags & I_FREE) {
    // race with evict
    panic("iput free");
  }
  if((ip->flags & I_VALID) == 0)
    panic("iput not valid");
  
  ip->flags |= I_FREE;

  ip->flags |= (I_BUSYR | I_BUSYW);
  ip->readbusy++;

  // XXX: use gc_delayed() to truncate the inode later.
  // flag it as a victim in the meantime.
  
  release(&ip->lock);
      
  itrunc(ip);
  ip->type = 0;
  ip->major = 0;
  ip->minor = 0;
  ip->gen += 1;
  iupdate(ip);
  
  ins->remove(make_pair(ip->dev, ip->inum), &ip);
  gc_delayed(ip);
  return;
}

// Increment reference count for ip.
// Returns ip to enable ip = idup(ip1) idiom.
struct inode*
idup(struct inode *ip)
{
  ip->inc();
  return ip;
}

// Lock the given inode.
// XXX why does ilock() read the inode from disk?
// why doesn't the iget() that allocated the inode cache entry
// read the inode from disk?
void
ilock(struct inode *ip, int writer)
{
  if(ip == 0 || !ip->valid())
    panic("ilock");

  acquire(&ip->lock);
  while(ip->flags & (I_BUSYW | (writer ? I_BUSYR : 0)))
    cv_sleep(&ip->cv, &ip->lock);
  ip->flags |= I_BUSYR | (writer ? I_BUSYW : 0);
  ip->readbusy++;
  release(&ip->lock);

  if((ip->flags & I_VALID) == 0)
    panic("ilock");
}

// Unlock the given inode.
void
iunlock(struct inode *ip)
{
  if(ip == 0 || !(ip->flags & (I_BUSYR | I_BUSYW)))
    panic("iunlock");

  acquire(&ip->lock);
  int lastreader = (--ip->readbusy);
  ip->flags &= ~(I_BUSYW | ((lastreader==0) ? I_BUSYR : 0));
  cv_wakeup(&ip->cv);
  release(&ip->lock);
}

void
iput(inode* ip, bool haveref)
{
  if (haveref)
    iput(ip);
}

// Caller holds reference to unlocked ip.  Drop reference.
void
iput(struct inode *ip)
{
  ip->dec();
}

// Common idiom: unlock, then put.
void
iunlockput(struct inode *ip)
{
  iunlock(ip);
  iput(ip);
}

//PAGEBREAK!
// Inode contents
//
// The contents (data) associated with each inode is stored
// in a sequence of blocks on the disk.  The first NDIRECT blocks
// are listed in ip->addrs[].  The next NINDIRECT blocks are 
// listed in the block ip->addrs[NDIRECT].

// Return the disk block address of the nth block in inode ip.
// If there is no such block, bmap allocates one.
static u32
bmap(struct inode *ip, u32 bn)
{
  struct buf *bp;
  u32 addr;

  if(bn < NDIRECT){
  retry0:
    if((addr = ip->addrs[bn]) == 0) {
      addr = balloc(ip->dev);
      if (!cmpxch(&ip->addrs[bn], (u32)0, addr)) {
        bfree(ip->dev, addr);
        goto retry0;
      }
    }
    return addr;
  }
  bn -= NDIRECT;

  if (bn >= NINDIRECT)
    panic("bmap: out of range");    

retry1:
  if (ip->iaddrs == nullptr) {
    if((addr = ip->addrs[NDIRECT]) == 0) {
      addr = balloc(ip->dev);
      if (!cmpxch(&ip->addrs[NDIRECT], (u32)0, addr)) {
        bfree(ip->dev, addr);
        goto retry1;
      }
    }

    volatile u32* iaddrs = (u32*)kmalloc(IADDRSSZ, "iaddrs");
    bp = bread(ip->dev, addr, 0);
    memmove((void*)iaddrs, bp->data, IADDRSSZ);
    brelse(bp, 0);
    if (!cmpxch(&ip->iaddrs, (volatile u32*)nullptr, iaddrs)) {
      bfree(ip->dev, addr);
      kmfree((void*)iaddrs, IADDRSSZ);
      goto retry1;
    }
  }

retry2:
  if ((addr = ip->iaddrs[bn]) == 0) {
    addr = balloc(ip->dev);
    if (!__sync_bool_compare_and_swap(&ip->iaddrs[bn], (u32)0, addr)) {
      bfree(ip->dev, addr);
      goto retry2;
    }
  }

  return addr;
}

// Truncate inode (discard contents).
// Only called after the last dirent referring
// to this inode has been erased on disk.
class diskblock : public rcu_freed {
 private:
  int _dev;
  u64 _block;

 public:
  diskblock(int dev, u64 block) : rcu_freed("diskblock"), _dev(dev), _block(block) {}
  virtual void do_gc() {
    bfree(_dev, _block);
    delete this;
  }

  NEW_DELETE_OPS(diskblock)
};

static void
itrunc(struct inode *ip)
{
  int i, j;
  struct buf *bp;
  u32 *a;

  for(i = 0; i < NDIRECT; i++){
    if(ip->addrs[i]){
      diskblock *db = new diskblock(ip->dev, ip->addrs[i]);
      gc_delayed(db);
      ip->addrs[i] = 0;
    }
  }
  
  if(ip->addrs[NDIRECT]){
    bp = bread(ip->dev, ip->addrs[NDIRECT], 0);
    a = (u32*)bp->data;
    for(j = 0; j < NINDIRECT; j++){
      if(a[j]) {
        diskblock *db = new diskblock(ip->dev, a[j]);
        gc_delayed(db);
      }
    }
    brelse(bp, 0);

    diskblock *db = new diskblock(ip->dev, ip->addrs[NDIRECT]);
    gc_delayed(db);
    ip->addrs[NDIRECT] = 0;
  }

  ip->size = 0;
  iupdate(ip);
}

// Copy stat information from inode.
void
stati(struct inode *ip, struct stat *st)
{
  if(ip->type == T_DEV){
    if(ip->major < 0 || ip->major >= NDEV || !devsw[ip->major].stat)
      memset(st, 0, sizeof(*st));
    else
      devsw[ip->major].stat(ip, st);
    return;
  }

  st->dev = ip->dev;
  st->ino = ip->inum;
  st->type = ip->type;
  st->nlink = ip->nlink();
  st->size = ip->size;
}

//PAGEBREAK!
// Read data from inode.
int
readi(struct inode *ip, char *dst, u32 off, u32 n)
{
  u32 tot, m;
  struct buf *bp;

  if(ip->type == T_DEV){
    if(ip->major < 0 || ip->major >= NDEV || !devsw[ip->major].read)
      return -1;
    return devsw[ip->major].read(ip, dst, off, n);
  }

  if(off > ip->size || off + n < off)
    return -1;
  if(off + n > ip->size)
    n = ip->size - off;

  for(tot=0; tot<n; tot+=m, off+=m, dst+=m){
    bp = bread(ip->dev, bmap(ip, off/BSIZE), 0);
    m = min(n - tot, BSIZE - off%BSIZE);
    memmove(dst, bp->data + off%BSIZE, m);
    brelse(bp, 0);
  }
  return n;
}

// PAGEBREAK!
// Write data to inode.
int
writei(struct inode *ip, const char *src, u32 off, u32 n)
{
  u32 tot, m;
  struct buf *bp;

  if(ip->type == T_DEV){
    if(ip->major < 0 || ip->major >= NDEV || !devsw[ip->major].write)
      return -1;
    return devsw[ip->major].write(ip, src, off, n);
  }

  if(off > ip->size || off + n < off)
    return -1;
  if(off + n > MAXFILE*BSIZE)
    n = MAXFILE*BSIZE - off;

  for(tot=0; tot<n; tot+=m, off+=m, src+=m){
    bp = bread(ip->dev, bmap(ip, off/BSIZE), 1);
    m = min(n - tot, BSIZE - off%BSIZE);
    memmove(bp->data + off%BSIZE, src, m);
    bwrite(bp);
    brelse(bp, 1);
  }

  if(n > 0 && off > ip->size){
    ip->size = off;
    iupdate(ip);
  }
  return n;
}

//PAGEBREAK!
// Directories

int
namecmp(const char *s, const char *t)
{
  return strncmp(s, t, DIRSIZ);
}

u64
namehash(const strbuf<DIRSIZ> &n)
{
  u64 h = 0;
  for (int i = 0; i < DIRSIZ && n._buf[i]; i++) {
    u64 c = n._buf[i];
    // Lifted from dcache.h in Linux v3.3
    h = (h + (c << 4) + (c >> 4)) * 11;
    // XXX(sbw) this doesn't seem to do well with the names
    // in dirbench (the low-order bits get clumped).
    // h = ((h << 8) ^ c) % 0xdeadbeef;
  }
  return h;
}

void
dir_init(struct inode *dp)
{
  if (dp->dir)
    return;
  if (dp->type != T_DIR)
    panic("dir_init not DIR");

  auto dir = new dirns();
  for (u32 off = 0; off < dp->size; off += BSIZE) {
    struct buf *bp = bread(dp->dev, bmap(dp, off / BSIZE), 0);
    for (struct dirent *de = (struct dirent *) bp->data;
	 de < (struct dirent *) (bp->data + BSIZE);
	 de++) {
      if (de->inum == 0)
	continue;

      dir->insert(strbuf<DIRSIZ>(de->name), de->inum);
    }
    brelse(bp, 0);
  }

  if (!cmpxch(&dp->dir, (decltype(dir)) 0, dir)) {
    // XXX free all the dirents
    delete dir;
  }
}

void
dir_flush(struct inode *dp)
{
  // assume already locked

  if (!dp->dir)
    return;

  u32 off = 0;
  dp->dir.load()->enumerate([dp, &off](const strbuf<DIRSIZ> &name, const u32 &inum)->bool{
      struct dirent de;
      strncpy(de.name, name._buf, DIRSIZ);
      de.inum = inum;
      if(writei(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
        panic("dir_flush_cb");
      off += sizeof(de);
      return false;
    });
  if (dp->size != off) {
    dp->size = off;
    iupdate(dp);
  }
}

// Look for a directory entry in a directory.
static inode*
__dirlookup(struct inode *dp, char *name, bool* haveref)
{
  // Assumes caller is holding a gc_epoch

  dir_init(dp);

  u32 inum = dp->dir.load()->lookup(strbuf<DIRSIZ>(name));
  if (inum == 0)
    return 0;
  return __iget(dp->dev, inum, haveref);
}

// Look for a directory entry in a directory.
struct inode*
dirlookup(struct inode *dp, char *name)
{
  dir_init(dp);

  u32 inum = dp->dir.load()->lookup(strbuf<DIRSIZ>(name));

  if (inum == 0)
    return 0;
  return iget(dp->dev, inum);
}

// Write a new directory entry (name, inum) into the directory dp.
int
dirlink(struct inode *dp, const char *name, u32 inum)
{
  dir_init(dp);

  //cprintf("dirlink: %x (%d): %s -> %d\n", dp, dp->inum, name, inum);
  return dp->dir.load()->insert(strbuf<DIRSIZ>(name), inum);
}

// Paths

// Copy the next path element from path into name.
// Update the pointer to the element following the copied one.
// The returned path has no leading slashes,
// so the caller can check *path=='\0' to see if the name is the last one.
// 
// If copied into name, return 1.
// If no name to remove, return 0.
// If the name is longer than DIRSIZ, return -1;
//
// Examples:
//   skipelem("a/bb/c", name) = "bb/c", setting name = "a"
//   skipelem("///a//bb", name) = "bb", setting name = "a"
//   skipelem("a", name) = "", setting name = "a"
//   skipelem("", name) = skipelem("////", name) = 0
//
static int
skipelem(const char **rpath, char *name)
{
  const char *path = *rpath;
  const char *s;
  int len;

  while(*path == '/')
    path++;
  if(*path == 0)
    return 0;
  s = path;
  while(*path != '/' && *path != 0)
    path++;
  len = path - s;
  if(len >= DIRSIZ)
    return -1;
  else {
    memmove(name, s, len);
    name[len] = 0;
  }
  while(*path == '/')
    path++;
  *rpath = path;
  return 1;
}

// Look up and return the inode for a path name.
// If parent != 0, return the inode for the parent and copy the final
// path element into name, which must have room for DIRSIZ bytes.
static struct inode*
__namex(inode *cwd, const char *path, int nameiparent, char *name,
        bool* retref)
{

  // Assumes caller is holding a gc_epoch

  struct inode *ip, *next;
  bool haveref;
  int r;

  if(*path == '/') 
    ip = the_root;
  else
    ip = cwd;
  haveref = false;

  while((r = skipelem(&path, name)) == 1){
    // XXX Doing this here requires some annoying reasoning about all
    // of the callers of namei/nameiparent.  Also, since the abstract
    // scope is implicit, it might be wrong (or non-existent) and
    // documenting the abstract object sets of each scope becomes
    // difficult and probably unmaintainable.  We have to compute this
    // information here because it's the only place that's canonical.
    // Maybe this should return the set of inodes traversed and let
    // the caller declare the variables?  Would it help for the caller
    // to pass in an abstract scope?
    mtreadavar("inode:%x.%x", ip->dev, ip->inum);
    if(ip->type == 0)
      panic("namex");
    if(ip->type != T_DIR)
      return 0;
    if(nameiparent && *path == '\0'){
      // Stop one level early.
      *retref = haveref;
      return ip;
    }

    bool nextref;
    if((next = __dirlookup(ip, name, &nextref)) == 0)
      return 0;
    if (haveref)
      iput(ip);

    haveref = nextref;
    ip = next;
  }

  if(r == -1 || nameiparent)
    return 0;

  // XXX write is necessary because of idup.  not logically required,
  // so we should replace this with mtreadavar() eventually, perhaps
  // once we implement sloppy counters for long-term inode refs.

  // mtreadavar("inode:%x.%x", ip->dev, ip->inum);
  mtwriteavar("inode:%x.%x", ip->dev, ip->inum);

  *retref = haveref;
  return ip;
}

inode*
__namei(inode *cwd, const char *path, bool* haveref)
{
  // Assumes caller is holding a gc_epoch
  char name[DIRSIZ];
  return __namex(cwd, path, 0, name, haveref);
}

inode*
namei(inode *cwd, const char *path)
{
  char name[DIRSIZ];
  bool haveref;

  inode* ip;
retry:
  {
    scoped_gc_epoch e;
    ip = __namex(cwd, path, 0, name, &haveref);
    if (ip != nullptr && !haveref)
      if (!ip->tryinc())
        goto retry;
  }

  return ip;
}

inode*
__nameiparent(inode *cwd, const char *path, char *name, bool* haveref)
{
  // Assumes caller is holding a gc_epoch
  return __namex(cwd, path, 1, name, haveref);
}


inode*
nameiparent(inode *cwd, const char *path, char *name)
{
  bool haveref;

  inode* ip;
retry:
  {
    scoped_gc_epoch e;
    ip = __namex(cwd, path, 1, name, &haveref);
    if (ip != nullptr && !haveref)
      if (!ip->tryinc())
        goto retry;
  }
  return ip;
}
