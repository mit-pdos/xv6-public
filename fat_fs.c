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
#include "fs.h"
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
  bp = bread(dev, 1);
  memmove(dbr, bp->data, sizeof(*dbr));
  brelse(bp);
}

// Zero a block.
static void
bzero(int dev, int bno)
{
}

// Blocks. 

// Allocate a zeroed disk block.
static uint
fat32_balloc(uint dev)
{
  readDbr(dev, &dbr);
  
}

// Free a disk block.
static void
bfree(int dev, uint b)
{
}


struct {
  struct spinlock lock;
  struct inode inode[NINODE];
} icache;

void
iinit(int dev)
{
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
iget(uint dev, uint inum)
{
}

struct inode*
idup(struct inode *ip)
{
}

void
ilock(struct inode *ip)
{
}

// Unlock the given inode.
void
iunlock(struct inode *ip)
{
}

void
iput(struct inode *ip)
{
}

// Common idiom: unlock, then put.
void
iunlockput(struct inode *ip)
{
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
