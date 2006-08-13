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

// these are inodes currently in use
// an entry is free if count == 0
struct inode inode[NINODE];
struct spinlock inode_table_lock;

uint rootdev = 1;

void
iinit(void)
{
  initlock(&inode_table_lock, "inode_table");
}

static uint 
balloc(uint dev) 
{
  int b;
  struct buf *bp;
  struct superblock *sb;
  int bi = 0;
  int size;
  int ninodes;
  uchar m;

  bp = bread(dev, 1);
  sb = (struct superblock *) bp->data;
  size = sb->size;
  ninodes = sb->ninodes;

  for (b = 0; b < size; b++) {
    if (b % BPB == 0) {
      brelse(bp);
      bp = bread(dev, BBLOCK(b, ninodes));
    }
    bi = b % BPB;
    m = 0x1 << (bi % 8);
    if ((bp->data[bi/8] & m) == 0) {  // is block free?
      break;
    }
  }
  if (b >= size)
    panic("balloc: out of blocks\n");

  bp->data[bi/8] |= 0x1 << (bi % 8);
  bwrite (bp, BBLOCK(b, ninodes));  // mark it allocated on disk
  brelse(bp);
  return b;
}

static void 
bfree(int dev, uint b)
{
  struct buf *bp;
  struct superblock *sb;
  int bi;
  int ninodes;
  uchar m;

  bp = bread(dev, 1);
  sb = (struct superblock *) bp->data;
  ninodes = sb->ninodes;
  brelse(bp);

  bp = bread(dev, b);
  memset(bp->data, 0, BSIZE);
  bwrite(bp, b);
  brelse(bp);

  bp = bread(dev, BBLOCK(b, ninodes));
  bi = b % BPB;
  m = ~(0x1 << (bi %8));
  bp->data[bi/8] &= m;
  bwrite (bp, BBLOCK(b, ninodes));  // mark it free on disk
  brelse(bp);
}

// returns an inode with busy set and incremented reference count.
struct inode *
iget(uint dev, uint inum)
{
  struct inode *ip, *nip;
  struct dinode *dip;
  struct buf *bp;

  acquire(&inode_table_lock);

 loop:
  nip = 0;
  for(ip = &inode[0]; ip < &inode[NINODE]; ip++){
    if(ip->count > 0 && ip->dev == dev && ip->inum == inum){
      if(ip->busy){
        sleep(ip, &inode_table_lock);
        goto loop;
      }
      ip->count++;
      ip->busy = 1;
      release(&inode_table_lock);
      return ip;
    }
    if(nip == 0 && ip->count == 0)
      nip = ip;
  }

  if(nip == 0)
    panic("out of inodes");

  nip->dev = dev;
  nip->inum = inum;
  nip->count = 1;
  nip->busy = 1;

  release(&inode_table_lock);

  bp = bread(dev, IBLOCK(inum));
  dip = &((struct dinode *)(bp->data))[inum % IPB];
  nip->type = dip->type;
  nip->major = dip->major;
  nip->minor = dip->minor;
  nip->nlink = dip->nlink;
  nip->size = dip->size;
  memmove(nip->addrs, dip->addrs, sizeof(nip->addrs));
  brelse(bp);

  return nip;
}

void 
iupdate (struct inode *ip)
{
  struct buf *bp;
  struct dinode *dip;

  bp = bread(ip->dev, IBLOCK(ip->inum));
  dip = &((struct dinode *)(bp->data))[ip->inum % IPB];
  dip->type = ip->type;
  dip->major = ip->major;
  dip->minor = ip->minor;
  dip->nlink = ip->nlink;
  dip->size = ip->size;
  memmove(dip->addrs, ip->addrs, sizeof(ip->addrs));
  bwrite (bp, IBLOCK(ip->inum));   // mark it allocated on the disk
  brelse(bp); 
}

struct inode *
ialloc(uint dev, short type)
{
  struct inode *ip;
  struct dinode *dip = 0;
  struct superblock *sb;
  int ninodes;
  int inum;
  struct buf *bp;

  bp = bread(dev, 1);
  sb = (struct superblock *) bp->data;
  ninodes = sb->ninodes;
  brelse(bp);

  for (inum = 1; inum < ninodes; inum++) {  // loop over inode blocks
    bp = bread(dev, IBLOCK(inum));
    dip = &((struct dinode *)(bp->data))[inum % IPB];
    if (dip->type == 0) {  // a free inode
      break;
    }
    brelse(bp);
  }
 
  if (inum >= ninodes) 
    panic ("ialloc: no inodes left\n");

  dip->type = type;
  bwrite (bp, IBLOCK(inum));   // mark it allocated on the disk
  brelse(bp);
  ip = iget (dev, inum);
  return ip;
}

static void
ifree(struct inode *ip)
{
  ip->type = 0;
  iupdate(ip);
}

void
ilock(struct inode *ip)
{
  if(ip->count < 1)
    panic("ilock");

  acquire(&inode_table_lock);

  while(ip->busy)
    sleep(ip, &inode_table_lock);
  ip->busy = 1;

  release(&inode_table_lock);
}

// caller is holding onto a reference to this inode, but no
// longer needs to examine or change it, so clear ip->busy.
void
iunlock(struct inode *ip)
{
  if(ip->busy != 1 || ip->count < 1)
    panic("iunlock");

  acquire(&inode_table_lock);

  ip->busy = 0;
  wakeup(ip);

  release(&inode_table_lock);
}

uint
bmap(struct inode *ip, uint bn)
{
  unsigned x;

  if(bn >= NDIRECT)
    panic("bmap 1");
  x = ip->addrs[bn];
  if(x == 0)
    panic("bmap 2");
  return x;
}

void 
iunlink(struct inode *ip)
{
  int i;

  // free inode, its blocks, and remove dir entry
  for (i = 0; i < NDIRECT; i++) {
    if (ip->addrs[i] != 0) {
      bfree(ip->dev, ip->addrs[i]);
      ip->addrs[i] = 0;
    }
  }
  ip->size = 0;
  ip->major = 0;
  ip->minor = 0;
  iupdate(ip);
  ifree(ip);  // is this the right order?
}

// caller is releasing a reference to this inode.
// you must have the inode lock.
void
iput(struct inode *ip)
{
  if(ip->count < 1 || ip->busy != 1)
    panic("iput");

  if ((ip->count == 1) && (ip->nlink == 0)) 
    iunlink(ip);

  acquire(&inode_table_lock);

  ip->count -= 1;
  ip->busy = 0;
  wakeup(ip);

  release(&inode_table_lock);
}

void
idecref(struct inode *ip)
{
  ilock(ip);
  iput(ip);
}

void
stati(struct inode *ip, struct stat *st)
{
  st->st_dev = ip->dev;
  st->st_ino = ip->inum;
  st->st_type = ip->type;
  st->st_nlink = ip->nlink;
  st->st_size = ip->size;
}

#define min(a, b) ((a) < (b) ? (a) : (b))

int
readi(struct inode *ip, char *dst, uint off, uint n)
{
  uint target = n, n1;
  struct buf *bp;

  if (ip->type == T_DEV) {
    if (ip->major < 0 || ip->major >= NDEV || !devsw[ip->major].d_read)
      return -1;
    return devsw[ip->major].d_read (ip->minor, dst, n);
  }

  while(n > 0 && off < ip->size){
    bp = bread(ip->dev, bmap(ip, off / BSIZE));
    n1 = min(n, ip->size - off);
    n1 = min(n1, BSIZE - (off % BSIZE));
    memmove(dst, bp->data + (off % BSIZE), n1);
    n -= n1;
    off += n1;
    dst += n1;
    brelse(bp);
  }

  return target - n;
}

int
writei(struct inode *ip, char *addr, uint off, uint n)
{
  if (ip->type == T_DEV) {
    if (ip->major < 0 || ip->major >= NDEV || !devsw[ip->major].d_write)
      return -1;
    return devsw[ip->major].d_write (ip->minor, addr, n);
  } else if (ip->type == T_FILE || ip->type == T_DIR) { // XXX dir here too?
    struct buf *bp;
    int r = 0;
    int m;
    int lbn;
    uint b;
    while (r < n) {
      lbn = off / BSIZE;
      if (lbn >= NDIRECT) return r;
      if (ip->addrs[lbn] == 0) {
	b = balloc(ip->dev);
	if (b <= 0) return r;
	ip->addrs[lbn] = b;
      }
      m = min(BSIZE - off % BSIZE, n-r);
      bp = bread(ip->dev, bmap(ip, off / BSIZE));
      memmove (bp->data + off % BSIZE, addr, m);
      bwrite (bp, bmap(ip, off/BSIZE));
      brelse (bp);
      r += m;
      off += m;
    }
    if (r > 0) {
      if (off > ip->size) {
	ip->size = off;
      }
      iupdate(ip);
    }
    return r;
  } else {
    panic ("writei: unknown type\n");
    return 0;
  }
}

// look up a path name, in one of three modes.
// NAMEI_LOOKUP: return locked target inode.
// NAMEI_CREATE: return locked parent inode.
//   but return 0 if name does exist.
// NAMEI_DELETE: return locked parent inode, offset of dirent in *ret_off.
//   return 0 if name doesn't exist.
struct inode *
namei(char *path, int mode, uint *ret_off)
{
  struct inode *dp;
  char *cp = path, *cp1;
  uint off, dev;
  struct buf *bp;
  struct dirent *ep;
  int i, atend;
  unsigned ninum;
  
  dp = iget(rootdev, 1);

  while(*cp == '/')
    cp++;

  while(1){
    if(*cp == '\0'){
      if(mode == NAMEI_LOOKUP)
        return dp;
      iput(dp);
      return 0;
    }

    if(dp->type != T_DIR){
      iput(dp);
      return 0;
    }

    for(off = 0; off < dp->size; off += BSIZE){
      bp = bread(dp->dev, bmap(dp, off / BSIZE));
      for(ep = (struct dirent *) bp->data;
          ep < (struct dirent *) (bp->data + BSIZE);
          ep++){
        if(ep->inum == 0) 
          continue;
        for(i = 0; i < DIRSIZ && cp[i] != '/' && cp[i]; i++)
          if(cp[i] != ep->name[i])
            break;
        if((cp[i] == '\0' || cp[i] == '/') && (i >= DIRSIZ || ep->name[i] == '\0')){
          off += (uchar*)ep - bp->data;
          ninum = ep->inum;
          brelse(bp);
          cp += i;
          goto found;
        }
      }
      brelse(bp);
    }
    atend = 1;
    for(cp1 = cp; *cp1; cp1++)
      if(*cp1 == '/')
        atend = 0;
    if(mode == NAMEI_CREATE && atend)
      return dp;

    iput(dp);
    return 0;

  found:
    if(mode == NAMEI_DELETE && *cp == '\0'){
      *ret_off = off;
      return dp;
    }
    dev = dp->dev;
    iput(dp);
    dp = iget(dev, ninum);
    if(dp->type == 0 || dp->nlink < 1)
      panic("namei");
    while(*cp == '/')
      cp++;
  }
}

void
wdir(struct inode *dp, char *name, uint ino)
{
  uint off;
  struct buf *bp = 0;
  struct dirent *ep = 0;
  int i;
  int lb;

  if(dp->size % BSIZE)
    dp->size += (BSIZE - dp->size % BSIZE);

  for(off = 0; off < dp->size; off += BSIZE) {
    bp = bread(dp->dev, bmap(dp, off / BSIZE));
    for(ep = (struct dirent *) bp->data;
	ep < (struct dirent *) (bp->data + BSIZE);
	ep++){
      if(ep->inum == 0)
	goto found;
    }
    brelse(bp);
  }
  lb = dp->size / BSIZE;
  if (lb >= NDIRECT) {
    panic ("wdir: too many entries");
  }
  dp->addrs[lb] = balloc(dp->dev);
  bp = bread(dp->dev, dp->addrs[lb]);
  memset(bp->data, 0, BSIZE);
  ep = (struct dirent *) (bp->data);
  dp->size += BSIZE;

 found:
  ep->inum = ino;
  for(i = 0; i < DIRSIZ && name[i]; i++)
    ep->name[i] = name[i];
  for( ; i < DIRSIZ; i++)
    ep->name[i] = '\0';
  bwrite (bp, bmap(dp, off/BSIZE));   // write directory block
  brelse(bp);
  iupdate(dp);
}

struct inode *
mknod(char *cp, short type, short major, short minor)
{
  struct inode *ip, *dp;

  if ((dp = namei(cp, NAMEI_CREATE, 0)) == 0)
    return 0;

  ip = ialloc(dp->dev, type);
  if (ip == 0) {
    iput(dp);
    return 0;
  }
  ip->major = major;
  ip->minor = minor;
  ip->size = 0;
  ip->nlink = 1;

  iupdate (ip);  // write new inode to disk
  
  wdir(dp, cp, ip->inum);
  iput(dp);
  return ip;
}

int
unlink(char *cp)
{
  struct inode *ip, *dp;
  struct dirent de;
  uint off, inum, dev;
  
  if ((dp = namei(cp, NAMEI_DELETE, &off)) == 0)
    return -1;

  dev = dp->dev;

  if(readi(dp, (char*)&de, off, sizeof(de)) != sizeof(de) || de.inum == 0)
    panic("unlink no entry");

  inum = de.inum;

  memset(&de, 0, sizeof(de));
  if(writei(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
    panic("unlink dir write");

  iupdate(dp);
  iput(dp);

  ip = iget(dev, inum);

  ip->nlink--;

  iupdate(ip);
  iput(ip);

  return 0;
}

int
link(char *name1, char *name2)
{
  struct inode *ip, *dp;

  if ((ip = namei(name1, NAMEI_LOOKUP, 0)) == 0)
    return -1;
  if(ip->type == T_DIR){
    iput(ip);
    return -1;
  }

  iunlock(ip);

  if ((dp = namei(name2, NAMEI_CREATE, 0)) == 0) {
    idecref(ip);
    return -1;
  }
  if(dp->dev != ip->dev){
    idecref(ip);
    iput(dp);
    return -1;
  }
  
  ilock(ip);
  ip->nlink += 1;
  iupdate (ip);

  wdir(dp, name2, ip->inum);
  iput(dp);
  iput(ip);

  return 0;
}
