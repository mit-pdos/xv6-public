#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#define stat xv6_stat  // avoid clash with host struct stat
#include "include/types.h"
#include "include/fs.h"
#include "include/stat.h"

int nblocks = 4067;
int ninodes = 2400;
int size = 4372;

int fsfd;
struct superblock sb;
char zeroes[512];
u32 freeblock;
u32 usedblocks;
u32 bitblocks;
u32 freeinode = 1;

void balloc(int);
void wsect(u32, void*);
void winode(u32, struct dinode*);
void rinode(u32 inum, struct dinode *ip);
void rsect(u32 sec, void *buf);
u32 ialloc(u16 type);
void iappend(u32 inum, void *p, int n);

// convert to intel byte order
u16
xshort(u16 x)
{
  u16 y;
  u8 *a = (u8*)&y;
  a[0] = x;
  a[1] = x >> 8;
  return y;
}

u32
xint(u32 x)
{
  u32 y;
  u8 *a = (u8*)&y;
  a[0] = x;
  a[1] = x >> 8;
  a[2] = x >> 16;
  a[3] = x >> 24;
  return y;
}

int
main(int argc, char *argv[])
{
  int i, cc, fd;
  u32 rootino, inum, off;
  struct dirent de;
  char buf[512];
  struct dinode din;

  if(argc < 2){
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    exit(1);
  }

  assert((512 % sizeof(struct dinode)) == 0);
  assert((512 % sizeof(struct dirent)) == 0);

  fsfd = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, 0666);
  if(fsfd < 0){
    perror(argv[1]);
    exit(1);
  }

  sb.size = xint(size);
  sb.nblocks = xint(nblocks); // so whole disk is size sectors
  sb.ninodes = xint(ninodes);

  bitblocks = (size+512*8-1)/(512*8);
  usedblocks = ninodes / IPB + 3 + bitblocks;
  freeblock = usedblocks;

  printf("used %d (bit %d ninode %zu) free %u total %d\n", usedblocks,
         bitblocks, ninodes/IPB + 1, freeblock, nblocks+usedblocks);

  assert(nblocks + usedblocks == size);

  for(i = 0; i < nblocks + usedblocks; i++)
    wsect(i, zeroes);

  memset(buf, 0, sizeof(buf));
  memmove(buf, &sb, sizeof(sb));
  wsect(1, buf);

  rootino = ialloc(T_DIR);
  assert(rootino == ROOTINO);

  bzero(&de, sizeof(de));
  de.inum = xshort(rootino);
  strcpy(de.name, ".");
  iappend(rootino, &de, sizeof(de));

  bzero(&de, sizeof(de));
  de.inum = xshort(rootino);
  strcpy(de.name, "..");
  iappend(rootino, &de, sizeof(de));

  for(i = 2; i < argc; i++){
    if((fd = open(argv[i], 0)) < 0){
      perror(argv[i]);
      exit(1);
    }

    // Lop off parent directories
    if (index(argv[i], '/'))
      argv[i] = rindex(argv[i], '/') + 1;

    // Skip leading _ in name when writing to file system.
    // The binaries are named _rm, _cat, etc. to keep the
    // build operating system from trying to execute them
    // in place of system binaries like rm and cat.
    if(argv[i][0] == '_')
      ++argv[i];

    inum = ialloc(T_FILE);

    bzero(&de, sizeof(de));
    de.inum = xshort(inum);
    strncpy(de.name, argv[i], DIRSIZ);
    iappend(rootino, &de, sizeof(de));

    while((cc = read(fd, buf, sizeof(buf))) > 0)
      iappend(inum, buf, cc);

    close(fd);
  }

  // fix size of root inode dir
  rinode(rootino, &din);
  off = xint(din.size);
  off = ((off/BSIZE) + 1) * BSIZE;
  din.size = xint(off);
  winode(rootino, &din);

  balloc(usedblocks);

  exit(0);
}

void
wsect(u32 sec, void *buf)
{
  if(lseek(fsfd, sec * 512L, 0) != sec * 512L){
    perror("lseek");
    exit(1);
  }
  if(write(fsfd, buf, 512) != 512){
    perror("write");
    exit(1);
  }
}

u32
i2b(u32 inum)
{
  return (inum / IPB) + 2;
}

void
winode(u32 inum, struct dinode *ip)
{
  char buf[512];
  u32 bn;
  struct dinode *dip;

  bn = i2b(inum);
  rsect(bn, buf);
  dip = ((struct dinode*)buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

void
rinode(u32 inum, struct dinode *ip)
{
  char buf[512];
  u32 bn;
  struct dinode *dip;

  bn = i2b(inum);
  rsect(bn, buf);
  dip = ((struct dinode*)buf) + (inum % IPB);
  *ip = *dip;
}

void
rsect(u32 sec, void *buf)
{
  if(lseek(fsfd, sec * 512L, 0) != sec * 512L){
    perror("lseek");
    exit(1);
  }
  if(read(fsfd, buf, 512) != 512){
    perror("read");
    exit(1);
  }
}

u32
ialloc(u16 type)
{
  u32 inum = freeinode++;
  struct dinode din;

  bzero(&din, sizeof(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.size = xint(0);
  din.gen = 1;
  winode(inum, &din);
  return inum;
}

void
balloc(int used)
{
  u8 buf[512];
  int i;

  printf("balloc: first %d blocks have been allocated\n", used);
  assert(used < 512*8);
  bzero(buf, 512);
  for(i = 0; i < used; i++){
    buf[i/8] = buf[i/8] | (0x1 << (i%8));
  }
  printf("balloc: write bitmap block at sector %zu\n", ninodes/IPB + 3);
  wsect(ninodes / IPB + 3, buf);
}

#define min(a, b) ((a) < (b) ? (a) : (b))

void
iappend(u32 inum, void *xp, int n)
{
  char *p = (char*)xp;
  u32 fbn, off, n1;
  struct dinode din;
  char buf[512];
  u32 indirect[NINDIRECT];
  u32 x;

  rinode(inum, &din);

  off = xint(din.size);
  while(n > 0){
    fbn = off / 512;
    assert(fbn < MAXFILE);
    if(fbn < NDIRECT){
      if(xint(din.addrs[fbn]) == 0){
        din.addrs[fbn] = xint(freeblock++);
        usedblocks++;
      }
      x = xint(din.addrs[fbn]);
    } else {
      if(xint(din.addrs[NDIRECT]) == 0){
        // printf("allocate indirect block\n");
        din.addrs[NDIRECT] = xint(freeblock++);
        usedblocks++;
      }
      // printf("read indirect block\n");
      rsect(xint(din.addrs[NDIRECT]), (char*)indirect);
      if(indirect[fbn - NDIRECT] == 0){
        indirect[fbn - NDIRECT] = xint(freeblock++);
        usedblocks++;
        wsect(xint(din.addrs[NDIRECT]), (char*)indirect);
      }
      x = xint(indirect[fbn-NDIRECT]);
    }
    n1 = min(n, (fbn + 1) * 512 - off);
    rsect(x, buf);
    bcopy(p, buf + off - (fbn * 512), n1);
    wsect(x, buf);
    n -= n1;
    off += n1;
    p += n1;
  }
  din.size = xint(off);
  winode(inum, &din);
}
