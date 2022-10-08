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
#include "sleeplock.h"
#include "fs.h"
#include "buf.h"
#include "file.h"
#include "mount.h"
#include "device.h"
#include "vfs_fs.h"

#define min(a, b) ((a) < (b) ? (a) : (b))

int             dirlink(struct vfs_inode*, char*, uint);
struct vfs_inode*   dirlookup(struct vfs_inode*, char*, uint*);
struct vfs_inode*   idup(struct vfs_inode*);
void            ilock(struct vfs_inode*);
void            iput(struct vfs_inode*);
void            iunlock(struct vfs_inode*);
void            iunlockput(struct vfs_inode*);
void            iupdate(struct vfs_inode*);
int             readi(struct vfs_inode*, char*, uint, uint);
void            stati(struct vfs_inode*, struct stat*);
int             writei(struct vfs_inode*, char*, uint, uint);
int             isdirempty (struct vfs_inode*);

static void            itrunc (struct vfs_inode *ip);

// Read the super block.
void
readsb(int dev, struct superblock *sb)
{
  struct buf *bp;

  bp = bread(dev, 1);
  memmove(sb, bp->data, sizeof(*sb));
  brelse(bp);
}

// Zero a block.
static void
bzero(int dev, int bno)
{
  struct buf *bp;

  bp = bread(dev, bno);
  memset(bp->data, 0, BSIZE);
  log_write(bp);
  brelse(bp);
}

// Blocks.

// Allocate a zeroed disk block.
static uint
balloc(uint dev)
{
  int b, bi, m;
  struct buf *bp;

    bp = 0;
    struct vfs_superblock *vfs_sb = getsuperblock(dev);
    struct superblock * sb = container_of(vfs_sb, struct superblock, vfs_sb);

    for (b = 0; b < sb->size; b += BPB) {
        bp = bread(dev, BBLOCK(b, *sb));
        for (bi = 0; bi < BPB && b + bi < sb->size; bi++) {
            m = 1 << (bi % 8);
            if ((bp->data[bi / 8] & m) == 0) {  // Is block free?
                bp->data[bi / 8] |= m;  // Mark block in use.
                log_write(bp);
                brelse(bp);
                bzero(dev, b + bi);
                return b + bi;
            }
        }
        brelse(bp);
    }
    panic("balloc: out of blocks");
}

// Free a disk block.
static void
bfree(int dev, uint b)
{
  struct buf *bp;
  int bi, m;

    struct vfs_superblock *vfs_sb = getsuperblock(dev);
    struct superblock * sb = container_of(vfs_sb, struct superblock, vfs_sb);
    readsb(dev, sb);
    bp = bread(dev, BBLOCK(b, *sb));
    bi = b % BPB;
    m = 1 << (bi % 8);
    if ((bp->data[bi / 8] & m) == 0)
        panic("freeing free block");
    bp->data[bi / 8] &= ~m;
    log_write(bp);
    brelse(bp);
}

// Inodes.
//
// An inode describes a single unnamed file.
// The inode disk structure holds metadata: the file's type,
// its size, the number of links referring to it, and the
// list of blocks holding the file's content.
//
// The inodes are laid out sequentially on disk at
// sb.startinode. Each inode has a number, indicating its
// position on the disk.
//
// The kernel keeps a cache of in-use inodes in memory
// to provide a place for synchronizing access
// to inodes used by multiple processes. The cached
// inodes include book-keeping information that is
// not stored on disk: ip->ref and ip->valid.
//
// An inode and its in-memory representation go through a
// sequence of states before they can be used by the
// rest of the file system code.
//
// * Allocation: an inode is allocated if its type (on disk)
//   is non-zero. ialloc() allocates, and iput() frees if
//   the reference and link counts have fallen to zero.
//
// * Referencing in cache: an entry in the inode cache
//   is free if ip->ref is zero. Otherwise ip->ref tracks
//   the number of in-memory pointers to the entry (open
//   files and current directories). iget() finds or
//   creates a cache entry and increments its ref; iput()
//   decrements ref.
//
// * Valid: the information (type, size, &c) in an inode
//   cache entry is only correct when ip->valid is 1.
//   ilock() reads the inode from
//   the disk and sets ip->valid, while iput() clears
//   ip->valid if ip->ref has fallen to zero.
//
// * Locked: file system code may only examine and modify
//   the information in an inode and its content if it
//   has first locked the inode.
//
// Thus a typical sequence is:
//   ip = iget(dev, inum)
//   ilock(ip)
//   ... examine and modify ip->xxx ...
//   iunlock(ip)
//   iput(ip)
//
// ilock() is separate from iget() so that system calls can
// get a long-term reference to an inode (as for an open file)
// and only lock it for short periods (e.g., in read()).
// The separation also helps avoid deadlock and races during
// pathname lookup. iget() increments ip->ref so that the inode
// stays cached and pointers to it remain valid.
//
// Many internal file system functions expect the caller to
// have locked the inodes involved; this lets callers create
// multi-step atomic operations.
//
// The icache.lock spin-lock protects the allocation of icache
// entries. Since ip->ref indicates whether an entry is free,
// and ip->dev and ip->inum indicate which i-node an entry
// holds, one must hold icache.lock while using any of those fields.
//
// An ip->lock sleep-lock protects all ip-> fields other than ref,
// dev, and inum.  One must hold ip->lock in order to
// read or write that inode's ip->valid, ip->size, ip->type, &c.

struct {
  struct spinlock lock;
  struct inode inode[NINODE];
} icache;

void
iinit(uint dev)
{
  int i = 0;
  
  initlock(&icache.lock, "icache");
  for(i = 0; i < NINODE; i++) {
    initsleeplock(&icache.inode[i].vfs_inode.lock, "inode");
  }

  fsinit(dev);
}

void fsinit(uint dev) {
    struct vfs_superblock *vfs_sb = getsuperblock(dev);
    struct superblock * sb = container_of(vfs_sb, struct superblock, vfs_sb);


    readsb(dev, sb);
    cprintf("sb: size %d nblocks %d ninodes %d nlog %d logstart %d\
 inodestart %d bmap start %d\n", sb->size, sb->nblocks,
            sb->vfs_sb.ninodes, sb->nlog, sb->logstart, sb->inodestart,
            sb->bmapstart);
}

//PAGEBREAK!
// Allocate an inode on device dev.
// Mark it as allocated by  giving it type type.
// Returns an unlocked but allocated and referenced inode.
struct vfs_inode *
ialloc(uint dev, short type) {
    int inum;
    struct buf *bp;
    struct dinode *dip;

    struct vfs_superblock *vfs_sb = getsuperblock(dev);
    struct superblock * sb = container_of(vfs_sb, struct superblock, vfs_sb);
    for (inum = 1; inum < sb->vfs_sb.ninodes; inum++) {
        bp = bread(dev, IBLOCK(inum, *sb));
        dip = (struct dinode *) bp->data + inum % IPB;
        if (dip->vfs_dinode.type == 0) {  // a free inode
            memset(dip, 0, sizeof(*dip));
            dip->vfs_dinode.type = type;
            log_write(bp);   // mark it allocated on the disk
            brelse(bp);
            return iget(dev, inum);
        }
        brelse(bp);
    }

    panic("ialloc: no inodes");
}

// Copy a modified in-memory inode to disk.
// Must be called after every change to an ip->xxx field
// that lives on disk, since i-node cache is write-through.
// Caller must hold ip->lock.
void
iupdate(struct vfs_inode *vfs_ip) {
    struct buf *bp;
    struct dinode *dip;
    struct inode * ip = container_of(vfs_ip, struct inode, vfs_inode);

    struct vfs_superblock *vfs_sb = getsuperblock(ip->vfs_inode.dev);
    struct superblock * sb = container_of(vfs_sb, struct superblock, vfs_sb);

    bp = bread(ip->vfs_inode.dev, IBLOCK(ip->vfs_inode.inum, *sb));
    dip = (struct dinode *) bp->data + ip->vfs_inode.inum % IPB;
    dip->vfs_dinode.type = ip->vfs_inode.type;
    dip->vfs_dinode.major = ip->vfs_inode.major;
    dip->vfs_dinode.minor = ip->vfs_inode.minor;
    dip->vfs_dinode.nlink = ip->vfs_inode.nlink;
    dip->size = ip->size;
    memmove(dip->addrs, ip->addrs, sizeof(ip->addrs));
    log_write(bp);
    brelse(bp);
}

// Find the inode with number inum on device dev
// and return the in-memory copy. Does not lock
// the inode and does not read it from disk.
struct vfs_inode *
iget(uint dev, uint inum) {
    struct inode *ip, *empty;

    acquire(&icache.lock);

    // Is the inode already cached?
    empty = 0;
    for (ip = &icache.inode[0]; ip < &icache.inode[NINODE]; ip++) {
        if (ip->vfs_inode.ref > 0 && ip->vfs_inode.dev == dev && ip->vfs_inode.inum == inum) {
            ip->vfs_inode.ref++;
            release(&icache.lock);
            return &ip->vfs_inode;
        }
        if (empty == 0 && ip->vfs_inode.ref == 0)    // Remember empty slot.
            empty = ip;
    }

    if(empty == 0 && ip->vfs_inode.ref == 0)    // Remember empty slot.
      empty = ip;

    // Recycle an inode cache entry.
    if (empty == 0)
        panic("iget: no inodes");

    deviceget(dev);
    ip = empty;
    ip->vfs_inode.dev = dev;
    ip->vfs_inode.inum = inum;
    ip->vfs_inode.ref = 1;
    ip->vfs_inode.valid = 0;

    /* Initiate inode operations for regular fs */
    ip->vfs_inode.i_op.idup = &idup;
    ip->vfs_inode.i_op.iupdate = &iupdate;
    ip->vfs_inode.i_op.iput = &iput;
    ip->vfs_inode.i_op.dirlink = &dirlink;
    ip->vfs_inode.i_op.dirlookup = &dirlookup;
    ip->vfs_inode.i_op.ilock = &ilock;
    ip->vfs_inode.i_op.iunlock = &iunlock;
    ip->vfs_inode.i_op.readi = &readi;
    ip->vfs_inode.i_op.stati = &stati;
    ip->vfs_inode.i_op.writei = &writei;
    ip->vfs_inode.i_op.iunlockput = &iunlockput;
    ip->vfs_inode.i_op.isdirempty = &isdirempty;

    release(&icache.lock);
    return &ip->vfs_inode;
}

// Increment reference count for ip.
// Returns ip to enable ip = idup(ip1) idiom.
struct vfs_inode *
idup(struct vfs_inode *ip) {
    acquire(&icache.lock);
    ip->ref++;
    release(&icache.lock);
    return ip;
}

// Lock the given inode.
// Reads the inode from disk if necessary.
void
ilock(struct vfs_inode *vfs_ip) {
    struct buf *bp;
    struct dinode *dip;
    struct inode *ip = container_of(vfs_ip, struct inode, vfs_inode);

    if (ip == 0 || ip->vfs_inode.ref < 1)
        panic("ilock");

    acquiresleep(&ip->vfs_inode.lock);

    if (ip->vfs_inode.valid == 0) {
        struct vfs_superblock *vfs_sb = getsuperblock(ip->vfs_inode.dev);
        struct superblock * sb = container_of(vfs_sb, struct superblock, vfs_sb);

        bp = bread(ip->vfs_inode.dev, IBLOCK(ip->vfs_inode.inum, *sb));
        dip = (struct dinode *) bp->data + ip->vfs_inode.inum % IPB;
        ip->vfs_inode.type = dip->vfs_dinode.type;
        ip->vfs_inode.major = dip->vfs_dinode.major;
        ip->vfs_inode.minor = dip->vfs_dinode.minor;
        ip->vfs_inode.nlink = dip->vfs_dinode.nlink;
        ip->size = dip->size;
        memmove(ip->addrs, dip->addrs, sizeof(ip->addrs));
        brelse(bp);
        ip->vfs_inode.valid = 1;
        if (ip->vfs_inode.type == 0)
            panic("ilock: no type");
    }
}

// Unlock the given inode.
void
iunlock(struct vfs_inode *ip) {
    if (ip == 0 || !holdingsleep(&ip->lock) || ip->ref < 1)
        panic("iunlock");

    releasesleep(&ip->lock);
}

// Drop a reference to an in-memory inode.
// If that was the last reference, the inode cache entry can
// be recycled.
// If that was the last reference and the inode has no links
// to it, free the inode (and its content) on disk.
// All calls to iput() must be inside a transaction in
// case it has to free the inode.
void
iput(struct vfs_inode *ip) {
    acquiresleep(&ip->lock);
    if (ip->valid && ip->nlink == 0) {
        acquire(&icache.lock);
        int r = ip->ref;
        release(&icache.lock);
        if (r == 1) {
            // inode has no links and no other references: truncate and free.
            itrunc(ip);
            ip->type = 0;
            ip->i_op.iupdate(ip);
            ip->valid = 0;
        }
    }
    releasesleep(&ip->lock);
    acquire(&icache.lock);

    ip->ref--;
    if (ip->ref == 0) {
        deviceput(ip->dev);
    }
    release(&icache.lock);
}

// Common idiom: unlock, then put.
void
iunlockput(struct vfs_inode *ip) {
    iunlock(ip);
    iput(ip);
}

//PAGEBREAK!
// Inode content
//
// The content (data) associated with each inode is stored
// in blocks on the disk. The first NDIRECT block numbers
// are listed in ip->addrs[].  The next NINDIRECT blocks are
// listed in block ip->addrs[NDIRECT].

// Return the disk block address of the nth block in inode ip.
// If there is no such block, bmap allocates one.
static uint
bmap(struct inode *ip, uint bn) {
    uint addr, *a;
    struct buf *bp;

    if (bn < NDIRECT) {
        if ((addr = ip->addrs[bn]) == 0)
            ip->addrs[bn] = addr = balloc(ip->vfs_inode.dev);
        return addr;
    }
    bn -= NDIRECT;

    if (bn < NINDIRECT) {
        // Load indirect block, allocating if necessary.
        if ((addr = ip->addrs[NDIRECT]) == 0)
            ip->addrs[NDIRECT] = addr = balloc(ip->vfs_inode.dev);
        bp = bread(ip->vfs_inode.dev, addr);
        a = (uint *) bp->data;
        if ((addr = a[bn]) == 0) {
            a[bn] = addr = balloc(ip->vfs_inode.dev);
            log_write(bp);
        }
        brelse(bp);
        return addr;
    }

    panic("bmap: out of range");
}

// Truncate inode (discard contents).
// Only called when the inode has no links
// to it (no directory entries referring to it)
// and has no in-memory reference to it (is
// not an open file or current directory).
static void
itrunc(struct vfs_inode *vfs_ip) {
    int i, j;
    struct buf *bp;
    uint *a;
    struct inode *ip = container_of(vfs_ip, struct inode, vfs_inode);

    for (i = 0; i < NDIRECT; i++) {
        if (ip->addrs[i]) {
            bfree(ip->vfs_inode.dev, ip->addrs[i]);
            ip->addrs[i] = 0;
        }
    }

    if (ip->addrs[NDIRECT]) {
        bp = bread(ip->vfs_inode.dev, ip->addrs[NDIRECT]);
        a = (uint *) bp->data;
        for (j = 0; j < NINDIRECT; j++) {
            if (a[j])
                bfree(ip->vfs_inode.dev, a[j]);
        }
        brelse(bp);
        bfree(ip->vfs_inode.dev, ip->addrs[NDIRECT]);
        ip->addrs[NDIRECT] = 0;
    }

    ip->size = 0;
    iupdate(&ip->vfs_inode);
}

// Copy stat information from inode.
// Caller must hold ip->lock.
void
stati(struct vfs_inode *vfs_ip, struct stat *st) {
    struct inode * ip = container_of(vfs_ip, struct inode, vfs_inode);

    st->dev = ip->vfs_inode.dev;
    st->ino = ip->vfs_inode.inum;
    st->type = ip->vfs_inode.type;
    st->nlink = ip->vfs_inode.nlink;
    st->size = ip->size;
}

//PAGEBREAK!
// Read data from inode.
// Caller must hold ip->lock.
int
readi(struct vfs_inode *vfs_ip, char *dst, uint off, uint n) {
    uint tot, m;
    struct buf *bp;
    struct inode * ip = container_of(vfs_ip, struct inode, vfs_inode);

  if(ip->vfs_inode.type == T_DEV){
    if(ip->vfs_inode.major < 0 || ip->vfs_inode.major >= NDEV
    || ip->vfs_inode.minor < 0 || ip->vfs_inode.minor >= MAX_TTY 
    || !devsw[ip->vfs_inode.major].read)
      return -1;
    return devsw[ip->vfs_inode.major].read(vfs_ip, dst, n);
  }

    if (off > ip->size || off + n < off)
        return -1;
    if (off + n > ip->size)
        n = ip->size - off;

    for (tot = 0; tot < n; tot += m, off += m, dst += m) {
        bp = bread(ip->vfs_inode.dev, bmap(ip, off / BSIZE));
        m = min(n - tot, BSIZE - off % BSIZE);
        memmove(dst, bp->data + off % BSIZE, m);
        brelse(bp);
    }
    return n;
}

// PAGEBREAK!
// Write data to inode.
// Caller must hold ip->lock.
int
writei(struct vfs_inode *vfs_ip, char *src, uint off, uint n) {
    uint tot, m;
    struct buf *bp;
    struct inode * ip = container_of(vfs_ip, struct inode, vfs_inode);

  if(ip->vfs_inode.type == T_DEV){
    if(ip->vfs_inode.major < 0 || ip->vfs_inode.major >= NDEV
    || ip->vfs_inode.minor < 0 || ip->vfs_inode.minor >= MAX_TTY 
    || !devsw[ip->vfs_inode.major].write)
      return -1;
    return devsw[ip->vfs_inode.major].write(vfs_ip, src, n);
  }

  if(off > ip->size || off + n < off)
    return -1;
  if(off + n > MAXFILE*BSIZE)
    return -1;

  for(tot=0; tot<n; tot+=m, off+=m, src+=m){
    bp = bread(ip->vfs_inode.dev, bmap(ip, off/BSIZE));
    m = min(n - tot, BSIZE - off%BSIZE);
    memmove(bp->data + off%BSIZE, src, m);
    log_write(bp);
    brelse(bp);
  }

  if(n > 0 && off > ip->size){
    ip->size = off;
    iupdate(&ip->vfs_inode);
  }
  return n;
}

//PAGEBREAK!
// Directories

// Is the directory dp empty except for "." and ".." ?
int
isdirempty(struct vfs_inode *vfs_dp)
{
  int off;
  struct dirent de;
  struct inode * dp = container_of(vfs_dp, struct inode, vfs_inode);

  for(off=2*sizeof(de); off<dp->size; off+=sizeof(de)) {
    if(dp->vfs_inode.i_op.readi(&dp->vfs_inode, (char*)&de, off, sizeof(de)) != sizeof(de))
      panic("isdirempty: readi");
    if(de.inum != 0)
      return 0;
  }

  return 1;
}

// Look for a directory entry in a directory.
// If found, set *poff to byte offset of entry.
struct vfs_inode *
dirlookup(struct vfs_inode *vfs_dp, char *name, uint *poff) {
    uint off, inum;
    struct dirent de;
    struct inode * dp = container_of(vfs_dp, struct inode, vfs_inode);

    if (dp->vfs_inode.type != T_DIR)
        panic("dirlookup not DIR");

    for (off = 0; off < dp->size; off += sizeof(de)) {
        if (readi(&dp->vfs_inode, (char *) &de, off, sizeof(de)) != sizeof(de))
            panic("dirlookup read");
        if (de.inum == 0)
            continue;
        if (vfs_namecmp(name, de.name) == 0) {
            // entry matches path element
            if (poff)
                *poff = off;
            inum = de.inum;
            return iget(dp->vfs_inode.dev, inum);
        }
    }

    return 0;
}

// Write a new directory entry (name, inum) into the directory dp.
int
dirlink(struct vfs_inode *vfs_dp, char *name, uint inum) {
    int off;
    struct dirent de;
    struct vfs_inode *ip;
    struct inode * dp = container_of(vfs_dp, struct inode, vfs_inode);

    // Check that name is not present.
    if ((ip = dirlookup(&dp->vfs_inode, name, 0)) != 0) {
        iput(ip);
        return -1;
    }

    // Look for an empty dirent.
    for (off = 0; off < dp->size; off += sizeof(de)) {
        if (readi(&dp->vfs_inode, (char *) &de, off, sizeof(de)) != sizeof(de))
            panic("dirlink read");
        if (de.inum == 0)
            break;
    }

    strncpy(de.name, name, DIRSIZ);
    de.inum = inum;
    if (writei(&dp->vfs_inode, (char *) &de, off, sizeof(de)) != sizeof(de))
        panic("dirlink");

    return 0;
}

//PAGEBREAK!
// Paths
struct vfs_inode *
initprocessroot(struct mount **mnt) {
    *mnt = getinitialrootmount();
    return iget(ROOTDEV, ROOTINO);
}
