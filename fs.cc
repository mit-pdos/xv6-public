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

extern "C" {
#include "types.h"
#include "stat.h"
#include "mmu.h"
#include "kernel.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"
#include "fs.h"
}

#include "buf.hh"
#include "file.hh"

extern "C" {
#include "cpu.h"
}

#define min(a, b) ((a) < (b) ? (a) : (b))
static void itrunc(struct inode*);

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
  return p._a ^ p._b;
}

static xns<pair<u32, u32>, inode*, ino_hash> *ins;

static struct { u32 x __mpalign__; } icache_free[NCPU];

void
initinode(void)
{
  ins = new xns<pair<u32, u32>, inode*, ino_hash>(false);
  for (int i = 0; i < NCPU; i++)
    icache_free[i].x = NINODE;
}

//PAGEBREAK!
// Allocate a new inode with the given type on device dev.
// Returns a locked inode.
struct inode*
ialloc(u32 dev, short type)
{
  int inum;
  struct buf *bp;
  struct dinode *dip;
  struct superblock sb;

  readsb(dev, &sb);
  for(inum = 1; inum < sb.ninodes; inum++){  // loop over inode blocks
    bp = bread(dev, IBLOCK(inum), 0);
    dip = (struct dinode*)bp->data + inum%IPB;
    int seemsfree = (dip->type == 0);
    brelse(bp, 0);
    if(seemsfree){
      // maybe this inode is free. look at it via the
      // inode cache to make sure.
      struct inode *ip = iget(dev, inum);
      ilock(ip, 1);
      if(ip->type == 0){
        ip->type = type;
        ip->gen += 1;
        if(ip->nlink || ip->size || ip->addrs[0])
          panic("ialloc not zeroed");
        iupdate(ip);
        return ip;
      }
      iunlockput(ip);
      //cprintf("ialloc oops %d\n", inum); // XXX harmless
    }
  }
  panic("ialloc: no inodes");
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
  dip->nlink = ip->nlink;
  dip->size = ip->size;
  dip->gen = ip->gen;
  memmove(dip->addrs, ip->addrs, sizeof(ip->addrs));
  bwrite(bp);
  brelse(bp, 1);
}

// Find the inode with number inum on device dev
// and return the in-memory copy.
// The inode is not locked, so someone else might
// be modifying it.
// But it has a ref count, so it won't be freed or reused.
// Though unlocked, all fields will be present,
// so looking a ip->inum and ip->gen are OK even w/o lock.
inode::inode()
  : rcu_freed("inode")
{
  dir = 0;
}

inode::~inode()
{
  if (dir) {
    dir->remove(strbuf<DIRSIZ>("."));
    dir->remove(strbuf<DIRSIZ>(".."));
    gc_delayed(dir);
    dir = 0;
  }

  destroylock(&lock);
}

struct inode*
iget(u32 dev, u32 inum)
{
  struct inode *ip;

 retry:
  // Try for cached inode.
  gc_begin_epoch();
  ip = ins->lookup(mkpair(dev, inum));
  if (ip) {
    // tricky: first bump ref, then check free flag
    __sync_fetch_and_add(&ip->ref, 1);
    if (ip->flags & I_FREE) {
      gc_end_epoch();
      __sync_sub_and_fetch(&ip->ref, 1);
      goto retry;
    }
    gc_end_epoch();
    if (!(ip->flags & I_VALID)) {
      acquire(&ip->lock);
      while((ip->flags & I_VALID) == 0)
	cv_sleep(&ip->cv, &ip->lock);
      release(&ip->lock);
    }
    return ip;
  }
  gc_end_epoch();

  // Allocate fresh inode cache slot.
 retry_evict:
  (void) 0;
  u32 cur_free = icache_free[mycpu()->id].x;
  if (cur_free == 0) {
    struct inode *victim = 0;
    ins->enumerate([&victim](const pair<u32, u32>&, inode* ip)->bool{
        if (ip->ref || ip->type == T_DIR)
          return false;

        acquire(&ip->lock);
        if (ip->ref == 0 && ip->type != T_DIR &&
            !(ip->flags & (I_FREE | I_BUSYR | I_BUSYW))) {
          victim = ip;
          return true;
        }

        release(&ip->lock);
        return false;
      });
    if (!victim)
      panic("iget out of space");
    // tricky: first flag as free, then check refcnt, then remove from ns
    victim->flags |= I_FREE;
    if (victim->ref > 0) {
      victim->flags &= ~(I_FREE);
      release(&victim->lock);
      goto retry_evict;
    }
    release(&victim->lock);
    ins->remove(mkpair(victim->dev, victim->inum), &victim);
    gc_delayed(victim);
  } else {
    if (!__sync_bool_compare_and_swap(&icache_free[mycpu()->id].x, cur_free, cur_free-1))
      goto retry_evict;
  }

  ip = new inode();
  ip->dev = dev;
  ip->inum = inum;
  ip->ref = 1;
  ip->flags = I_BUSYR | I_BUSYW;
  ip->readbusy = 1;
  snprintf(ip->lockname, sizeof(ip->lockname), "cv:ino:%d", ip->inum);
  initlock(&ip->lock, ip->lockname+3, LOCKSTAT_FS);
  initcondvar(&ip->cv, ip->lockname);
  if (ins->insert(mkpair(ip->dev, ip->inum), ip) < 0) {
    gc_delayed(ip);
    goto retry;
  }
  
  struct buf *bp = bread(ip->dev, IBLOCK(ip->inum), 0);
  struct dinode *dip = (struct dinode*)bp->data + ip->inum%IPB;
  ip->type = dip->type;
  ip->major = dip->major;
  ip->minor = dip->minor;
  ip->nlink = dip->nlink;
  ip->size = dip->size;
  ip->gen = dip->gen;
  memmove(ip->addrs, dip->addrs, sizeof(ip->addrs));
  brelse(bp, 0);
  ip->flags |= I_VALID;

  iunlock(ip);

  return ip;
}

// Increment reference count for ip.
// Returns ip to enable ip = idup(ip1) idiom.
struct inode*
idup(struct inode *ip)
{
  __sync_fetch_and_add(&ip->ref, 1);
  return ip;
}

// Lock the given inode.
// XXX why does ilock() read the inode from disk?
// why doesn't the iget() that allocated the inode cache entry
// read the inode from disk?
void
ilock(struct inode *ip, int writer)
{
  if(ip == 0 || ip->ref < 1)
    panic("ilock");

  acquire(&ip->lock);
  while(ip->flags & (I_BUSYW | (writer ? I_BUSYR : 0)))
    cv_sleep(&ip->cv, &ip->lock);
  ip->flags |= I_BUSYR | (writer ? I_BUSYW : 0);
  __sync_fetch_and_add(&ip->readbusy, 1);
  release(&ip->lock);

  if((ip->flags & I_VALID) == 0)
    panic("ilock");
}

// Unlock the given inode.
void
iunlock(struct inode *ip)
{
  if(ip == 0 || !(ip->flags & (I_BUSYR | I_BUSYW)) || ip->ref < 1)
    panic("iunlock");

  acquire(&ip->lock);
  int lastreader = __sync_sub_and_fetch(&ip->readbusy, 1);
  ip->flags &= ~(I_BUSYW | ((lastreader==0) ? I_BUSYR : 0));
  cv_wakeup(&ip->cv);
  release(&ip->lock);
}

// Caller holds reference to unlocked ip.  Drop reference.
void
iput(struct inode *ip)
{
  if(__sync_sub_and_fetch(&ip->ref, 1) == 0) {
    if (ip->nlink)
      return;
    acquire(&ip->lock);
    if (ip->ref == 0 && ip->nlink == 0) {
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
      if (ip->ref > 0) {
	ip->flags &= ~(I_FREE);
	release(&ip->lock);
	return;
      }

      ip->flags |= (I_BUSYR | I_BUSYW);
      __sync_fetch_and_add(&ip->readbusy, 1);

      release(&ip->lock);

      itrunc(ip);
      ip->type = 0;
      ip->major = 0;
      ip->minor = 0;
      ip->gen += 1;
      iupdate(ip);

      ins->remove(mkpair(ip->dev, ip->inum), &ip);
      gc_delayed(ip);
      __sync_fetch_and_add(&icache_free[mycpu()->id].x, 1);
      return;
    }
    release(&ip->lock);
  }
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
  u32 addr, *a;
  struct buf *bp;

  if(bn < NDIRECT){
    if((addr = ip->addrs[bn]) == 0)
      ip->addrs[bn] = addr = balloc(ip->dev);
    return addr;
  }
  bn -= NDIRECT;

  if(bn < NINDIRECT){
    // Load indirect block, allocating if necessary.
    if((addr = ip->addrs[NDIRECT]) == 0)
      ip->addrs[NDIRECT] = addr = balloc(ip->dev);
    bp = bread(ip->dev, addr, 1);
    a = (u32*)bp->data;
    if((addr = a[bn]) == 0){
      a[bn] = addr = balloc(ip->dev);
      bwrite(bp);
    }
    brelse(bp, 1);
    return addr;
  }

  panic("bmap: out of range");
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
  virtual ~diskblock() {
    bfree(_dev, _block);
  }
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
  st->nlink = ip->nlink;
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
writei(struct inode *ip, char *src, u32 off, u32 n)
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
  return n._buf[0];   /* XXX */
}

void
dir_init(struct inode *dp)
{
  if (dp->dir)
    return;
  if (dp->type != T_DIR)
    panic("dir_init not DIR");

  auto dir = new xns<strbuf<DIRSIZ>, u32, namehash>(false);
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
  if (!__sync_bool_compare_and_swap(&dp->dir, 0, dir)) {
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
  dp->dir->enumerate([dp, &off](const strbuf<DIRSIZ> &name, const u32 &inum)->bool{
      struct dirent de;
      strncpy(de.name, name._buf, DIRSIZ);
      de.inum = inum;
      if(writei(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
        panic("dir_flush_cb");
      off += sizeof(de);
      return false;
    });
}

// Look for a directory entry in a directory.
struct inode*
dirlookup(struct inode *dp, char *name)
{
  dir_init(dp);

  u32 inum = dp->dir->lookup(strbuf<DIRSIZ>(name));

  //cprintf("dirlookup: %x (%d): %s -> %d\n", dp, dp->inum, name, inum);
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
  return dp->dir->insert(strbuf<DIRSIZ>(name), inum);
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
namex(const char *path, int nameiparent, char *name)
{
  struct inode *ip, *next;
  int r;

  gc_begin_epoch();
  if(*path == '/') 
    ip = iget(ROOTDEV, ROOTINO);
  else
    ip = idup(myproc()->cwd);

  while((r = skipelem(&path, name)) == 1){
    next = 0;
    if(next == 0){
      if(ip->type == 0)
        panic("namex");
      if(ip->type != T_DIR){
        iput(ip);
	gc_end_epoch();
        return 0;
      }
      if(nameiparent && *path == '\0'){
        // Stop one level early.
	gc_end_epoch();
        return ip;
      }
      if((next = dirlookup(ip, name)) == 0){
        iput(ip);
	gc_end_epoch();
        return 0;
      }
      iput(ip);
    }
    ip = next;
  }
  if(r == -1 || nameiparent){
    iput(ip);
    gc_end_epoch();
    return 0;
  }
  gc_end_epoch();
  return ip;
}

struct inode*
namei(const char *path)
{
  char name[DIRSIZ];
  struct inode *r = namex(path, 0, name);
  //cprintf("namei: %s -> %x (%d)\n", path, r, r?r->inum:0);
  return r;
}

struct inode*
nameiparent(char *path, char *name)
{
  return namex(path, 1, name);
}

