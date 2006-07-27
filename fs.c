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
struct spinlock inode_table_lock;

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
  nip->nlink = dip->nlink;
  nip->size = dip->size;
  memmove(nip->addrs, dip->addrs, sizeof(nip->addrs));
  brelse(bp);

  return nip;
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
iincref(struct inode *ip)
{
  acquire(&inode_table_lock);

  ip->count += 1;

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
