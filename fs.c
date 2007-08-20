// File system implementation.
// 
// Four layers: 
//   + Blocks: allocator for raw disk blocks.
//   + Files: inode allocator, reading, writing, metadata.
//   + Directories: inode with special contents (list of other inodes!)
//   + Names: paths like /usr/rtm/xv6/fs.c for convenient naming.
//
// Disk layout is: superblock, inodes, disk bitmap, data blocks.

// TODO: Check locking!

#include "types.h"
#include "stat.h"
#include "param.h"
#include "x86.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "spinlock.h"
#include "buf.h"
#include "fs.h"
#include "fsvar.h"
#include "dev.h"

#define min(a, b) ((a) < (b) ? (a) : (b))

static void ifree(struct inode*);

// Blocks. 

// Allocate a disk block.
static uint
balloc(uint dev)
{
  int b, bi, m, ninodes, size;
  struct buf *bp;
  struct superblock *sb;

  bp = bread(dev, 1);
  sb = (struct superblock*) bp->data;
  size = sb->size;
  ninodes = sb->ninodes;

  for(b = 0; b < size; b++) {
    if(b % BPB == 0) {
      brelse(bp);
      bp = bread(dev, BBLOCK(b, ninodes));
    }
    bi = b % BPB;
    m = 0x1 << (bi % 8);
    if((bp->data[bi/8] & m) == 0) {  // is block free?
      bp->data[bi/8] |= 0x1 << (bi % 8);
      bwrite(bp, BBLOCK(b, ninodes));  // mark it allocated on disk
      brelse(bp);
      return b;
    }
  }
  panic("balloc: out of blocks");
}

// Free a disk block.
static void
bfree(int dev, uint b)
{
  struct buf *bp;
  struct superblock *sb;
  int bi, m, ninodes;

  bp = bread(dev, 1);
  sb = (struct superblock*) bp->data;
  ninodes = sb->ninodes;
  brelse(bp);

  bp = bread(dev, b);
  memset(bp->data, 0, BSIZE);
  bwrite(bp, b);
  brelse(bp);

  bp = bread(dev, BBLOCK(b, ninodes));
  bi = b % BPB;
  m = 0x1 << (bi % 8);
  bp->data[bi/8] &= ~m;
  bwrite(bp, BBLOCK(b, ninodes));  // mark it free on disk
  brelse(bp);
}

// Inodes
//
// The inodes are laid out sequentially on disk immediately after
// the superblock.  The kernel keeps a cache of the in-use
// on-disk structures to provide a place for synchronizing access
// to inodes shared between multiple processes.
// 
// ip->ref counts the number of references to this
// inode; references are typically kept in struct file and in cp->cwd.
// When ip->ref falls to zero, the inode is no longer cached.
// It is an error to use an inode without holding a reference to it.
//
// Inodes can be marked busy, just like bufs, meaning
// that some process has logically locked the inode, and other processes
// are not allowed to look at it.  Because the locking can last for 
// a long time (for example, during a disk access), we use a flag
// like in buffer cache, not spin locks.  The inode should always be
// locked during modifications to it.

struct {
  struct spinlock lock;
  struct inode inode[NINODE];
} icache;

void
iinit(void)
{
  initlock(&icache.lock, "icache.lock");
}

// Find the inode with number inum on device dev
// and return an in-memory copy.  Loads the inode
// from disk into the in-core table if necessary.
// The returned inode is locked and has its ref count incremented.
// Caller must iput the return value when done with it.
struct inode*
iget(uint dev, uint inum)
{
  struct inode *ip, *empty;
  struct dinode *dip;
  struct buf *bp;

  acquire(&icache.lock);

 loop:
  // Try for cached inode.
  empty = 0;
  for(ip = &icache.inode[0]; ip < &icache.inode[NINODE]; ip++){
    if(ip->ref > 0 && ip->dev == dev && ip->inum == inum){
      if(ip->busy){
        sleep(ip, &icache.lock);
        goto loop;
      }
      ip->ref++;
      ip->busy = 1;
      release(&icache.lock);
      return ip;
    }
    if(empty == 0 && ip->ref == 0)    // Remember empty slot.
      empty = ip;
  }

  // Allocate fresh inode.
  if(empty == 0)
    panic("iget: no inodes");

  ip = empty;
  ip->dev = dev;
  ip->inum = inum;
  ip->ref = 1;
  ip->busy = 1;
  release(&icache.lock);

  bp = bread(dev, IBLOCK(inum));
  dip = &((struct dinode*)(bp->data))[inum % IPB];
  ip->type = dip->type;
  ip->major = dip->major;
  ip->minor = dip->minor;
  ip->nlink = dip->nlink;
  ip->size = dip->size;
  memmove(ip->addrs, dip->addrs, sizeof(ip->addrs));
  brelse(bp);

  return ip;
}

// Iget the inode for the file system root (/).
struct inode*
igetroot(void)
{
  return iget(ROOTDEV, 1);
}

// Lock the given inode.
void
ilock(struct inode *ip)
{
  if(ip->ref < 1)
    panic("ilock");

  acquire(&icache.lock);
  while(ip->busy)
    sleep(ip, &icache.lock);
  ip->busy = 1;
  release(&icache.lock);
}

// Unlock the given inode.
void
iunlock(struct inode *ip)
{
  if(ip->busy != 1 || ip->ref < 1)
    panic("iunlock");

  acquire(&icache.lock);
  ip->busy = 0;
  wakeup(ip);
  release(&icache.lock);
}

// Unlock inode and drop reference.
void
iput(struct inode *ip)
{
  if(ip->ref < 1 || ip->busy != 1)
    panic("iput");

  if((ip->ref == 1) && (ip->nlink == 0)) {
    itrunc(ip);
    ifree(ip);
  }

  acquire(&icache.lock);
  ip->ref -= 1;
  ip->busy = 0;
  wakeup(ip);
  release(&icache.lock);
}

// Increment reference count for ip.
// Returns ip to enable ip = iincref(ip1) idiom.
struct inode*
iincref(struct inode *ip)
{
  ilock(ip);
  ip->ref++;
  iunlock(ip);
  return ip;
}

// Caller holds reference to unlocked ip.
// Drop reference.
void
idecref(struct inode *ip)
{
  ilock(ip);
  iput(ip);
}

// Allocate a new inode with the given type on device dev.
struct inode*
ialloc(uint dev, short type)
{
  struct inode *ip;
  struct dinode *dip;
  struct superblock *sb;
  int ninodes;
  int inum;
  struct buf *bp;

  bp = bread(dev, 1);
  sb = (struct superblock*)bp->data;
  ninodes = sb->ninodes;
  brelse(bp);

  for(inum = 1; inum < ninodes; inum++) {  // loop over inode blocks
    bp = bread(dev, IBLOCK(inum));
    dip = &((struct dinode*)(bp->data))[inum % IPB];
    if(dip->type == 0) {  // a free inode
      memset(dip, 0, sizeof(*dip));
      dip->type = type;
      bwrite(bp, IBLOCK(inum));   // mark it allocated on the disk
      brelse(bp);
      ip = iget(dev, inum);
      return ip;
    }
    brelse(bp);
  }
  panic("ialloc: no inodes");
}

// Copy inode, which has changed, from memory to disk.
void
iupdate(struct inode *ip)
{
  struct buf *bp;
  struct dinode *dip;

  bp = bread(ip->dev, IBLOCK(ip->inum));
  dip = &((struct dinode*)(bp->data))[ip->inum % IPB];
  dip->type = ip->type;
  dip->major = ip->major;
  dip->minor = ip->minor;
  dip->nlink = ip->nlink;
  dip->size = ip->size;
  memmove(dip->addrs, ip->addrs, sizeof(ip->addrs));
  bwrite(bp, IBLOCK(ip->inum));
  brelse(bp);
}

// Free (delete) the given inode.
// Caller must have ip locked.
static void
ifree(struct inode *ip)
{
  ip->type = 0;
  iupdate(ip);
}

// Inode contents
//
// The contents (data) associated with each inode is stored
// in a sequence of blocks on the disk.  The first NDIRECT blocks
// are stored in ip->addrs[].  The next NINDIRECT blocks are 
// listed in the block ip->addrs[INDIRECT].

// Return the disk block address of the nth block in inode ip.
// If there is no such block: if alloc is set, allocate one, else return -1.
uint
bmap(struct inode *ip, uint bn, int alloc)
{
  uint addr, *a;
  struct buf *bp;

  if(bn < NDIRECT) {
    if((addr = ip->addrs[bn]) == 0) {
      if(!alloc)
        return -1;
      ip->addrs[bn] = addr = balloc(ip->dev);
    }
    return addr;
  }
  bn -= NDIRECT;

  if(bn < NINDIRECT) {
    // Load indirect block, allocating if necessary.
    if((addr = ip->addrs[INDIRECT]) == 0) {
      if(!alloc)
        return -1;
      ip->addrs[INDIRECT] = addr = balloc(ip->dev);
    }
    bp = bread(ip->dev, addr);
    a = (uint*)bp->data;
  
    if((addr = a[bn]) == 0) {
      if(!alloc) {
        brelse(bp);
        return -1;
      }
      a[bn] = addr = balloc(ip->dev);
      bwrite(bp, ip->addrs[INDIRECT]);
    }
    brelse(bp);
    return addr;
  }

  panic("bmap: out of range");
}

// Truncate inode (discard contents).
void
itrunc(struct inode *ip)
{
  int i, j;
  struct buf *bp;
  uint *a;

  for(i = 0; i < NDIRECT; i++) {
    if(ip->addrs[i]) {
      bfree(ip->dev, ip->addrs[i]);
      ip->addrs[i] = 0;
    }
  }
  
  if(ip->addrs[INDIRECT]) {
    bp = bread(ip->dev, ip->addrs[INDIRECT]);
    a = (uint*)bp->data;
    for(j = 0; j < NINDIRECT; j++) {
      if(a[j])
        bfree(ip->dev, a[j]);
    }
    brelse(bp);
    ip->addrs[INDIRECT] = 0;
  }

  ip->size = 0;
  iupdate(ip);
}

// Copy stat information from inode.
void
stati(struct inode *ip, struct stat *st)
{
  st->dev = ip->dev;
  st->ino = ip->inum;
  st->type = ip->type;
  st->nlink = ip->nlink;
  st->size = ip->size;
}

// Read data from inode.
int
readi(struct inode *ip, char *dst, uint off, uint n)
{
  uint tot, m;
  struct buf *bp;

  if(ip->type == T_DEV) {
    if(ip->major < 0 || ip->major >= NDEV || !devsw[ip->major].read)
      return -1;
    return devsw[ip->major].read(ip->minor, dst, n);
  }

  if(off + n < off)
    return -1;
  if(off + n > ip->size)
    n = ip->size - off;

  for(tot=0; tot<n; tot+=m, off+=m, dst+=m) {
    bp = bread(ip->dev, bmap(ip, off/BSIZE, 0));
    m = min(n - tot, BSIZE - off%BSIZE);
    memmove(dst, bp->data + off%BSIZE, m);
    brelse(bp);
  }
  return n;
}

// Write data to inode.
int
writei(struct inode *ip, char *src, uint off, uint n)
{
  uint tot, m;
  struct buf *bp;

  if(ip->type == T_DEV) {
    if(ip->major < 0 || ip->major >= NDEV || !devsw[ip->major].write)
      return -1;
    return devsw[ip->major].write(ip->minor, src, n);
  }

  if(off + n < off)
    return -1;
  if(off + n > MAXFILE*BSIZE)
    n = MAXFILE*BSIZE - off;

  for(tot=0; tot<n; tot+=m, off+=m, src+=m) {
    bp = bread(ip->dev, bmap(ip, off/BSIZE, 1));
    m = min(n - tot, BSIZE - off%BSIZE);
    memmove(bp->data + off%BSIZE, src, m);
    bwrite(bp, bmap(ip, off/BSIZE, 0));
    brelse(bp);
  }

  if(n > 0 && off > ip->size) {
    ip->size = off;
    iupdate(ip);
  }
  return n;
}

// Directories
//
// Directories are just inodes (files) filled with dirent structures.

// Look for a directory entry in a directory.
// If not found, return -1.
// If found:
//   set *poff to the byte offset of the directory entry
//   set *pinum to the inode number
//   return 0.
int
dirlookup(struct inode *dp, char *name, int namelen, uint *poff, uint *pinum)
{
  uint off;
  struct buf *bp;
  struct dirent *de;

  if(dp->type != T_DIR)
    return -1;

  for(off = 0; off < dp->size; off += BSIZE){
    bp = bread(dp->dev, bmap(dp, off / BSIZE, 0));
    for(de = (struct dirent*) bp->data;
        de < (struct dirent*) (bp->data + BSIZE);
        de++){
      if(de->inum == 0)
        continue;
      if(memcmp(name, de->name, namelen) == 0 &&
         (namelen == DIRSIZ || de->name[namelen]== 0)){
        // entry matches path element
        if(poff)
          *poff = off + (uchar*)de - bp->data;
        if(pinum)
          *pinum = de->inum;
        brelse(bp);
        return 0;
      }
    }
    brelse(bp);
  }
  return -1;
}

// Write a new directory entry (name, ino) into the directory dp.
// Caller must have locked dp.
void
dirwrite(struct inode *dp, char *name, int namelen, uint ino)
{
  int off;
  struct dirent de;

  // Look for an empty dirent.
  for(off = 0; off < dp->size; off += sizeof(de)){
    if(readi(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
      panic("dirwrite read");
    if(de.inum == 0)
      break;
  }

  de.inum = ino;
  if(namelen > DIRSIZ)
    namelen = DIRSIZ;
  memmove(de.name, name, namelen);
  memset(de.name+namelen, 0, DIRSIZ-namelen);

  if(writei(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
    panic("dirwrite");
}

// Create a new inode named name inside dp
// and return its locked inode structure.
// If name already exists, return 0.
struct inode*
dircreat(struct inode *dp, char *name, int namelen, short type, short major, short minor)
{
  struct inode *ip;

  ip = ialloc(dp->dev, type);
  if(ip == 0)
    return 0;
  ip->major = major;
  ip->minor = minor;
  ip->size = 0;
  ip->nlink = 1;
  iupdate(ip);

  dirwrite(dp, name, namelen, ip->inum);

  return ip;
}

// Paths

// Skip over the next path element in path, 
// saving it in *name and its length in *len.
// Return a pointer to the element after that
// (after any trailing slashes).
// Thus the caller can check whether *path=='\0'
// to see whether the name just removed was
// the last one.  
// If there is no name to remove, return 0.
//
// Examples:
//   skipelem("a/bb/c") = "bb/c", with *name = "a/bb/c", len=1
//   skipelem("///a/bb") = "b", with *name="a/bb", len=1
//   skipelem("") = skipelem("////") = 0
//
static char*
skipelem(char *path, char **name, int *len)
{
  while(*path == '/')
    path++;
  if(*path == 0)
    return 0;
  *name = path;
  while(*path != '/' && *path != 0)
    path++;
  *len = path - *name;
  while(*path == '/')
    path++;
  return path;
}

// look up a path name, in one of three modes.
// NAMEI_LOOKUP: return locked target inode.
// NAMEI_CREATE: return locked parent inode.
//   return 0 if name does exist.
//   *ret_last points to last path component (i.e. new file name).
//   *ret_ip points to the the name that did exist, if it did.
//   *ret_ip and *ret_last may be zero even if return value is zero.
// NAMEI_DELETE: return locked parent inode, offset of dirent in *ret_off.
//   return 0 if name doesn't exist.
struct inode*
_namei(char *path, int parent, char **pname, int *pnamelen)
{
  struct inode *dp;
  char *name;
  int namelen;
  uint off, dev, inum;

  if(*path == '/')
    dp = igetroot();
  else {
    dp = iincref(cp->cwd);
    ilock(dp);
  }

  while((path = skipelem(path, &name, &namelen)) != 0){
    // Truncate names in path to DIRSIZ chars.
    if(namelen > DIRSIZ)
      namelen = DIRSIZ;

    if(dp->type != T_DIR)
      goto fail;
    
    if(parent && *path == '\0'){
      // Stop one level early.
      *pname = name;
      *pnamelen = namelen;
      return dp;
    }

    if(dirlookup(dp, name, namelen, &off, &inum) < 0)
      goto fail;

    dev = dp->dev;
    iput(dp);
    dp = iget(dev, inum);
    if(dp->type == 0 || dp->nlink < 1)
      panic("namei");
  }
  if(parent)
    return 0;
  return dp;

fail:
  iput(dp);
  return 0;
}

struct inode*
namei(char *path)
{
  return _namei(path, 0, 0, 0);
}

struct inode*
nameiparent(char *path, char **name, int *namelen)
{
  return _namei(path, 1, name, namelen);
}



// Create the path and return its locked inode structure.
// If cp already exists, return 0.
struct inode*
mknod(char *path, short type, short major, short minor)
{
  struct inode *ip, *dp;
  char *name;
  int namelen;

  if((dp = nameiparent(path, &name, &namelen)) == 0)
    return 0;
  if(dirlookup(dp, name, namelen, 0, 0) >= 0){
    iput(dp);
    return 0;
  }
  ip = dircreat(dp, name, namelen, type, major, minor);
  iput(dp);
  return ip;
}

// Unlink the inode named cp.
int
unlink(char *path)
{
  struct inode *ip, *dp;
  struct dirent de;
  uint off, inum, dev;
  char *name;
  int namelen;

  if((dp = nameiparent(path, &name, &namelen)) == 0)
    return -1;
  if(dirlookup(dp, name, namelen, &off, 0) < 0){
    iput(dp);
    return -1;
  }

  if(readi(dp, (char*)&de, off, sizeof(de)) != sizeof(de) || de.inum == 0)
    panic("unlink no entry");
  
  // Cannot remove "." or ".." - the 2 and 3 count the trailing NUL.
  if(memcmp(de.name, ".", 2) == 0 || memcmp(de.name, "..", 3) == 0){
    iput(dp);
    return -1;
  }

  inum = de.inum;

  memset(&de, 0, sizeof(de));
  if(writei(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
    panic("unlink dir write");

  dev = dp->dev;
  iput(dp);

  ip = iget(dev, inum);
  if(ip->nlink < 1)
    panic("unlink nlink < 1");
  ip->nlink--;
  iupdate(ip);
  iput(ip);

  return 0;
}

// Create the path new as a link to the same inode as old.
int
link(char *old, char *new)
{
  struct inode *ip, *dp;
  char *name;
  int namelen;

  if((ip = namei(old)) == 0)
    return -1;
  if(ip->type == T_DIR){
    iput(ip);
    return -1;
  }
  iunlock(ip);
  
  if((dp = nameiparent(new, &name, &namelen)) == 0){
    idecref(ip);
    return -1;
  }
  if(dirlookup(dp, name, namelen, 0, 0) >= 0){
    iput(dp);
    idecref(ip);
    return -1;
  }
  if(dp->dev != ip->dev){
    idecref(ip);
    iput(dp);
    return -1;
  }

  // LOCKING ERROR HERE!  TWO LOCKS HELD AT ONCE.
  ilock(ip);
  ip->nlink++;
  iupdate(ip);

  dirwrite(dp, name, namelen, ip->inum);
  iput(dp);
  iput(ip);

  return 0;
}
