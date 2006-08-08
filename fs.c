#include "types.h"
#include "param.h"
#include "x86.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "spinlock.h"
#include "buf.h"
#include "fs.h"
#include "fsvar.h"

// these are inodes currently in use
// an entry is free if count == 0
struct inode inode[NINODE];
struct spinlock inode_table_lock = { "inode_table" };

uint rootdev = 1;

// returns an inode with busy set and incremented reference count.
struct inode *
iget(uint dev, uint inum)
{
  struct inode *ip, *nip = 0;
  struct dinode *dip;
  struct buf *bp;

  acquire(&inode_table_lock);

 loop:
  for(ip = &inode[0]; ip < &inode[NINODE]; ip++){
    if(ip->count > 0 && ip->dev == dev && ip->inum == inum){
      if(ip->busy){
        sleep(ip, &inode_table_lock);
        goto loop;
      }
      ip->count++;
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

  bp = bread(dev, inum / IPB + 2);
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

// allocate an inode on disk
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
  sb = (struct superblock *) bp;
  ninodes = sb->ninodes;
  brelse(bp);
 
  for (inum = 1; inum < ninodes; inum++) {  // loop over inode blocks
    bp = bread(dev, inum / IPB + 2);
    dip = &((struct dinode *)(bp->data))[inum % IPB];
    if (dip->type == 0) {  // a free inode
      break;
    }
    brelse(bp);
  }
 
  if (inum >= ninodes) {
    cprintf ("ialloc: no inodes left\n");
    return 0;
  }

  cprintf ("ialloc: %d\n", inum);
  dip->type = type;
  bwrite (dev, bp, inum / IPB + 2);   // mark it allocated on the disk
  brelse(bp);
  ip = iget (dev, inum);
  return ip;
}

void 
iupdate (struct inode *ip)
{
  struct buf *bp;
  struct dinode *dip;

  bp = bread(ip->dev, ip->inum / IPB + 2);
  dip = &((struct dinode *)(bp->data))[ip->inum % IPB];
  dip->type = ip->type;
  dip->major = ip->major;
  dip->minor = ip->minor;
  dip->nlink = ip->nlink;
  dip->size = ip->size;
  bwrite (ip->dev, bp, ip->inum / IPB + 2);   // mark it allocated on the disk
  brelse(bp); 
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
  if(ip->busy != 1)
    panic("iunlock");

  acquire(&inode_table_lock);

  ip->busy = 0;
  wakeup(ip);

  release(&inode_table_lock);
}

// caller is releasing a reference to this inode.
// you must have the inode lock.
void
iput(struct inode *ip)
{
  if(ip->count < 1 || ip->busy != 1)
    panic("iput");

  acquire(&inode_table_lock);

  ip->count -= 1;
  ip->busy = 0;
  wakeup(ip);

  release(&inode_table_lock);
}

void
idecref(struct inode *ip)
{
  acquire(&inode_table_lock);

  if(ip->count < 1)
    panic("idecref");

  ip->count -= 1;

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

#define min(a, b) ((a) < (b) ? (a) : (b))

int
readi(struct inode *ip, void *xdst, uint off, uint n)
{
  char *dst = (char *) xdst;
  uint target = n, n1;
  struct buf *bp;

  while(n > 0 && off < ip->size){
    bp = bread(ip->dev, bmap(ip, off / 512));
    n1 = min(n, ip->size - off);
    n1 = min(n1, 512 - (off % 512));
    memmove(dst, bp->data + (off % 512), n1);
    n -= n1;
    off += n1;
    dst += n1;
    brelse(bp);
  }

  return target - n;
}

struct inode *
namei(char *path)
{
  struct inode *dp;
  char *cp = path;
  uint off, dev;
  struct buf *bp;
  struct dirent *ep;
  int i;
  unsigned ninum;

  dp = iget(rootdev, 1);

  while(*cp == '/')
    cp++;

  while(1){
    if(*cp == '\0')
      return dp;

    if(dp->type != T_DIR){
      iput(dp);
      return 0;
    }

    for(off = 0; off < dp->size; off += 512){
      bp = bread(dp->dev, bmap(dp, off / 512));
      for(ep = (struct dirent *) bp->data;
          ep < (struct dirent *) (bp->data + 512);
          ep++){
        if(ep->inum == 0) 
          continue;
        for(i = 0; i < DIRSIZ && cp[i] != '/' && cp[i]; i++)
          if(cp[i] != ep->name[i])
            break;
        if((cp[i] == '\0' || cp[i] == '/') && (i >= DIRSIZ || ep->name[i] == '\0')){
          ninum = ep->inum;
          brelse(bp);
          cp += i;
          goto found;
        }
      }
      brelse(bp);
    }
    iput(dp);
    return 0;

  found:
    dev = dp->dev;
    iput(dp);
    dp = iget(dev, ninum);
    while(*cp == '/')
      cp++;
  }
}

struct inode *
mknod(struct inode *dp, char *cp, short type, short major, short minor)
{
  struct inode *ip;
  struct dirent *ep = 0;
  int off;
  int i;
  struct buf *bp = 0;

  cprintf("mknod: %s %d %d %d\n", cp, type, major, minor);

  ip = ialloc(dp->dev, type);
  if (ip == 0) return 0;
  ip->major = major;
  ip->minor = minor;

  for(off = 0; off < dp->size; off += 512) {
    bp = bread(dp->dev, bmap(dp, off / 512));
    for(ep = (struct dirent *) bp->data;
	ep < (struct dirent *) (bp->data + 512);
	ep++){
      if(ep->inum == 0) {
	goto found;
      }
    }
    brelse(bp);
  }
  panic("mknod: no dir entry free\n");

 found:
  ep->inum = ip->inum;
  for(i = 0; i < DIRSIZ && cp[i]; i++) ep->name[i] = cp[i];
  bwrite (dp->dev, bp, bmap(dp, off/512));   // write directory
  brelse(bp);
  iupdate (ip);
  return ip;
}
