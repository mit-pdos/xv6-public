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
  cprintf("bn %d off %d\n", inum / IPB + 2, (unsigned)dip - (unsigned)bp->data);
  brelse(bp);

  return nip;
}

void
iput(struct inode *ip)
{
  acquire(&inode_table_lock);

  ip->count -= 1;
  ip->busy = 0;
  wakeup(ip);

  release(&inode_table_lock);
}
