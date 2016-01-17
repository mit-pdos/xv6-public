// File system implementation.  Five layers:
//   + Blocks: allocator for raw disk blocks.
//   + Log: crash recovery for multi-step updates.
//   + Files: inode allocator, reading, writing, metadata.
//   + Directories: inode with special contents (list of other inodes!)
//   + Names: paths like /usr/rtm/xv6/fs.c for convenient naming.
//
// This file contains the low-level file system manipulation 
// routines.  The (higher-level) system call implementations
// are in sysfile.c.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "fat_fs.h"
#include "buf.h"
#include "file.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
static void itrunc(struct inode*);
struct FAT32_DBR dbr;   // there should be one per dev, but we run with one dev

// Read the super block.
void
readDbr(int dev, struct FAT32_DBR *dbr)
{
  struct buf *bp;
  bp = bread(dev, 0);
  memmove(dbr, bp->data, sizeof(*dbr));
  brelse(bp);
}

// Zero a block.
static void
bzero(int dev, int bno)
{
}

// Blocks. 
uint getFATStart(uint cnum, uint *offset)
{
  uint ret;
  *offset = (cnum * 4);
  ret = dbr.RsvdSecCnt + (*offset / dbr.BytesPerSec)
  *offset %= dbr.BytesPerSec;
  return ret;
}

void updateFATs(struct buf* sp){
  struct buf *tp;
  int i, offset;
  for (i = 1, offset = dbr.FATSz32; i < dbr.NumFATs; i++, offset += dbr.FATSz32) {
    tp = bread(sp->dev, sp->sector + offset);
    memmove(tp->data, sp->data, 512);
    bwrite(tp);
    brelse(tp);
  }
}

uint getFirstSector(uint cnum)
{
  return (cnum - 2) * dbr->SecPerClus + dbr->RsvdSecCnt + dbr->NumFATs * dbr->FATSz32;
}
// Allocate a zeroed disk cluster.
static uint
fat32_balloc(uint dev)
{
  uint cnum, nowSec, lastSec;
  struct buf *bp, *bfsi;
  struct FSInfo *fsi;
  readDbr(dev, &dbr);
  bfsi = bread(dev, dbr.FSInfo);
  fsi = (struct FSInfo*)bfsi->data;
  lastSec = 0;
  for (cnum = fsi->Nxt_Free + 1; cnum < dbr.TotSec32 / dbr.SecPerClus; cnum++){
    uint offset;
    nowSec = getFATStart(cnum, &offset);
    if (nowSec != lastSec){
      if (bp)
        brelse(bp);
      bp = bread(dev, nowSec);
      lastSec = nowSec;
    }
    if (!*(uint *)(bp->data + offset)){
      *(uint *)(bp->data + offset) = LAST_FAT_VALUE;
      fsi->Nxt_Free++;
      fsi->Free_Count--;
      updateFATs(bp);
      bwrite(bp);
      brelse(bp);
      bwrite(bfsi);
      brelse(bfsi);
      return cnum;
    }
  }
  for (cnum = 2; cnum <= fsi->Nxt_Free; cnum++){
    uint offset;
    nowSec = getFATStart(cnum, &offset);
    if (nowSec != lastSec){
      if (bp)
        brelse(bp);
      bp = bread(dev, nowSec);
      lastSec = nowSec;
    }
    if (!*(uint *)(bp->data + offset)){
      *(uint *)(bp->data + offset) = LAST_FAT_VALUE;
      fsi->Nxt_Free = cnum;
      fsi->Free_Count--;
      updateFATs(bp);
      bwrite(bp);
      brelse(bp);
      bwrite(bfsi);
      brelse(bfsi);
      return cnum;
    }
  }
  panic("balloc: out of clusters");
}

// Free a disk cluster.
static void
bfree(int dev, uint cnum)
{
    struct buf *bp;
    uint offset, nowSec;
    readDbr(dev, &dbr);
    nowSec = getFATStart(cnum, &offset);
    bp = bread(dev, nowSec);
    *(uint *)(bp->data + offset) = 0;
    bwrite(bp);
    brelse(bp);
}


struct {
  struct spinlock lock;
  struct inode inode[NINODE];
} icache;

void
iinit(int dev)
{
    initlock(&icache.lock, "icache");
    readDbr(dev, &dbr);
    cprintf("dbr: BytesPerSec: %d   SecPerClus: %d   NumFATs: %d   TotSec32:   %d", dbr.BytesPerSec, dbr.SecPerClus, dbr.NumFATs, dbr.TotSec32);
}

static struct inode* iget(uint dev, uint inum);

struct inode*
ialloc(uint dev, short type)
{
}

void
iupdate(struct inode *ip)
{
}

static struct inode*
iget(uint dev, uint inum, uint dirCluster)
{
  struct inode *ip, *empty;
  acquire(&icache.lock);

  // Is the inode already cached?
  empty = 0;
  for(ip = &icache.inode[0]; ip < &icache.inode[NINODE]; ip++){
    if(ip->ref > 0 && ip->dev == dev && ip->inum == inum){
      ip->ref++;
      release(&icache.lock);
      return ip;
    }
    if(empty == 0 && ip->ref == 0)    // Remember empty slot.
      empty = ip;
  }

  // Recycle an inode cache entry.
  if(empty == 0)
    panic("iget: no inodes");

  ip = empty;
  ip->dev = dev;
  ip->inum = inum;
  ip->ref = 1;
  ip->flags = 0;
  ip->dirCluster = dirCluster;
  release(&icache.lock);

  return ip;
}

struct inode*
idup(struct inode *ip)
{
  acquire(&icache.lock);
  ip->ref++;
  release(&icache.lock);
  return ip;
}

void
ilock(struct inode *ip)
{
  struct buf *bp;
  struct direntry *dip;
  uint dirCluster = ip->dirCluster;

  if(ip == 0 || ip->ref < 1)
    panic("ilock");

  acquire(&icache.lock);
  while(ip->flags & I_BUSY)
    sleep(ip, &icache.lock);
  ip->flags |= I_BUSY;
  release(&icache.lock);

  if(!(ip->flags & I_VALID)){
    
    bp = bread(ip->dev, IBLOCK(ip->inum, sb));
    dip = (struct dinode*)bp->data + ip->inum%IPB;
    ip->type = dip->type;
    ip->major = dip->major;
    ip->minor = dip->minor;
    ip->nlink = dip->nlink;
    ip->size = dip->size;
    memmove(ip->addrs, dip->addrs, sizeof(ip->addrs));
    brelse(bp);
    ip->flags |= I_VALID;
    if(ip->type == 0)
      panic("ilock: no type");
  }

}

// Unlock the given inode.
void
iunlock(struct inode *ip)
{
  if(ip == 0 || !(ip->flags & I_BUSY) || ip->ref < 1)
    panic("iunlock");

  acquire(&icache.lock);
  ip->flags &= ~I_BUSY;
  wakeup(ip);
  release(&icache.lock);
}

void
iput(struct inode *ip)
{
}

// Common idiom: unlock, then put.
void
iunlockput(struct inode *ip)
{
  iunlock(ip);
  iput(ip);
}

static uint
bmap(struct inode *ip, uint bn)
{
}

static void
itrunc(struct inode *ip)
{
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

int
readi(struct inode *ip, char *dst, uint off, uint n)
{
}

int
writei(struct inode *ip, char *src, uint off, uint n)
{
}

//PAGEBREAK!
// Directories

int
namecmp(const char *s, const char *t)
{
  return strncmp(s, t, DIRSIZ);
}

// Look for a directory entry in a directory.
// If found, set *poff to byte offset of entry.
struct inode*
dirlookup(struct inode *dp, char *name, uint *poff)
{
}

// Write a new directory entry (name, inum) into the directory dp.
int
dirlink(struct inode *dp, char *name, uint inum)
{
}

static char*
skipelem(char *path, char *name)
{
}

static struct inode*
namex(char *path, int nameiparent, char *name)
{
}

struct inode*
namei(char *path)
{
  char name[DIRSIZ];
  return namex(path, 0, name);
}

struct inode*
nameiparent(char *path, char *name)
{
  return namex(path, 1, name);
}
