#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#define stat xv6_stat  // avoid clash with host struct stat
#include "types.h"
#include "fat_fs.h"
#include "stat.h"
#include "param.h"
#include "x86.h"

#ifndef static_assert
#define static_assert(a, b) do { switch (0) case 0: case (a): ; } while (0)
#endif

#define min(a, b) ((a) < (b) ? (a) : (b))

// Disk layout:
// [ DBR sector | retain sector | FAT sectors | data sectors ]

int nDBR = 1;     // number of DBR sector DBR占有扇区数
int nRetain = RETAINSEC;  // number of retain sectors 保留扇区数
// number of a FAT sectors 单个FAT表所占扇区数
int nFAT = ((FSSIZE - nDBR - nRetain) * 4 + SECPERCLUS + SECSIZE) / (SECSIZE + 8/SECPERCLUS);
int nData = FSSIZE - nDBR - nRetain - 2 * nFAT; // number of data sectors 数据扇区数
int nDataClus = nData / SECPERCLUS; // 簇数量

int fsfd; // fs.img的fd
struct FAT32_DBR fatDBR;
struct FSInfo fsi;
uint freeClusIdx = 8; // FAT表中空闲位置的index
uint freeClusNum = 0; // 空闲簇号
uint fstClusSec = nDBR + nRetain + nFAT * 2;  // 第一个簇对应的第一个扇区号

/*
* 函数功能：向sec号扇区写入数据buf
* 参数说明：sec扇区号,buf数据
*/
void wsect(uint sec, void *buf);
/*
* 函数功能：从sec号扇区中读取数据到buf
* 参数说明：sec扇区号,buf存储数据
*/
void rsect(uint sec, void *buf);
/*
* 函数功能：簇号对应的起始扇区号
* 参数说明：clus簇号
*/
uint clus2sec(uint clus);
/* 函数功能：分配空闲簇号(在FAT表中查找) */
uint cnallloc();
/* 函数功能：分配FAT表中空闲位置 */
uint fatidxalloc();
/* 清零扇区和簇 */
void szero(uint sec);
void czero(uint clus);
/*
* 函数功能：向簇中写入数据
* 参数说明：clus簇号,offsec簇中扇区偏移量[0,1...SECPERCLUS),buf数据
*/
void appendBuf(uint clus, uint offsec, void *buf);
/* 函数功能：时间获取函数*/
uchar getSecond();
uchar getMinute();
uchar getHour();
uchar getDay();
uchar getMonth();
uchar getYear();
/* 函数功能: 初始化函数 */
void initDBR();
void initFSI();
// convert to intel byte order
ushort xshort(ushort x);
uint xint(uint x);


int main(int argc, char *argv[])
{
  // 局部变量


  static_assert(sizeof(int) == 4, "Integers must be 4 bytes!");

  if(argc < 2){
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    exit(1);
  }

  // 是否有需要断言
  // ?

  fsfd = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, 0666);
  if(fsfd < 0){
    perror(argv[1]);
    exit(1);
  }

  // 初始化sb
  // 1 fs block = 1 disk sector
  nmeta = 2 + nlog + ninodeblocks + nbitmap;
  nblocks = FSSIZE - nmeta;

  sb.size = xint(FSSIZE);
  sb.nblocks = xint(nblocks);
  sb.ninodes = xint(NINODES);
  sb.nlog = xint(nlog);
  sb.logstart = xint(2);
  sb.inodestart = xint(2+nlog);
  sb.bmapstart = xint(2+nlog+ninodeblocks);

  printf("nmeta %d (boot, super, log blocks %u inode blocks %u, bitmap blocks %u) blocks %d total %d\n",
         nmeta, nlog, ninodeblocks, nbitmap, nblocks, FSSIZE);

  freeblock = nmeta;     // the first free block that we can allocate

  for(i = 0; i < FSSIZE; i++)
    wsect(i, zeroes);

  // 写入sb
  memset(buf, 0, sizeof(buf));
  memmove(buf, &sb, sizeof(sb));
  wsect(1, buf);

  // 写入根目录
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

  // 写入其他文件
  for(i = 2; i < argc; i++){
    assert(index(argv[i], '/') == 0);

    if((fd = open(argv[i], 0)) < 0){
      perror(argv[i]);
      exit(1);
    }
    
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

  balloc(freeblock);

  exit(0);
}

void　wsect(uint sec, void *buf)
{
  if(lseek(fsfd, sec * SECSIZE, 0) != sec * SECSIZE){
    perror("lseek");
    exit(1);
  }
  if(write(fsfd, buf, SECSIZE) != SECSIZE){
    perror("write");
    exit(1);
  }
}

void　rsect(uint sec, void *buf)
{
  if(lseek(fsfd, sec * SECSIZE, 0) != sec * SECSIZE){
    perror("lseek");
    exit(1);
  }
  if(read(fsfd, buf, SECSIZE) != SECSIZE){
    perror("read");
    exit(1);
  }
}

uint clus2sec(uint clus)
{
  return (fstClusSec + clus * SECPERCLUS);
}

void szero(uint sec)
{
  uchar buf[SECSIZE];
  memset(buf, 0, SECSIZE);
  wsect(sec, buf);
}

void czero(uint clus)
{
  uint sec = clus2sec(clus);
  uint end = sec + SECPERCLUS;
  for(; sec < end; sec++) {
    szero(sec);
  }
}

uint cnallloc()
{

}

uint fatidxalloc()
{

}

void appendBuf(uint clus, uint offsec, void *buf)
{

}

void initDBR() 
{
  fatDBR.BytesPerSec = xshort(SECSIZE);
  fatDBR.SecPerClus  = SECPERCLUS;
  fatDBR.RsvdSecCnt  = xshort(nDBR + nRetain);
  fatDBR.NumFATs     = 2;
  fatDBR.HiddSec     = xint(0);
  fatDBR.TotSec32    = xint(FSSIZE);
  fatDBR.FATSz32     = xint(nFAT);
  fatDBR.RootClus    = xint(0);
  fatDBR.FSInfo      = xshort(1);
  fatDBR.BkBootSec   = xshort(6);
}

void initFSI() 
{
  fsi.LeadSig    = xint(nDBR);
  fsi.Free_Count = xint(nDataClus);
  fsi.Nxt_Free   = xint(0);
  // fsi结束标记
}

uchar getSecond()
{
  uchar data;
  outb(0x70, 0x00);
  data = inb(0x71);
  return (data >> 4) * 10 + (data & 0xf);
}

uchar getMinute()
{
  uchar data;
  outb(0x70, 0x02);
  data = inb(0x71);
  return (data >> 4) * 10 + (data & 0xf);
}

uchar getHour()
{
  uchar data;
  outb(0x70, 0x04);
  data = inb(0x71);
  return ((data >> 4) * 10 + (data & 0xf) + 8) % 24;
}

uchar getDay()
{
 uchar data;
 outb(0x70, 0x07);
 data = inb(0x71);
 return (data >> 4) * 10 + (data & 0xf);
}

uchar getMonth()
{
  uchar data;
  outb(0x70, 0x08);
  data = inb(0x71);
  return (data >> 4) * 10 + (data & 0xf);
}

uchar getYear()
{
  ushort data;
  outb(0x70, 0x09);
  data = inb(0x71);
  return (data >> 4) * 10 + (data & 0xf);
}

ushort xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar*)&y;
  a[0] = x;
  a[1] = x >> 8;
  return y;
}

uint xint(uint x)
{
  uint y;
  uchar *a = (uchar*)&y;
  a[0] = x;
  a[1] = x >> 8;
  a[2] = x >> 16;
  a[3] = x >> 24;
  return y;
}











void
winode(uint inum, struct dinode *ip)
{
  char buf[BSIZE];
  uint bn;
  struct dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((struct dinode*)buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

void
rinode(uint inum, struct dinode *ip)
{
  char buf[BSIZE];
  uint bn;
  struct dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((struct dinode*)buf) + (inum % IPB);
  *ip = *dip;
}


uint
ialloc(ushort type)
{
  uint inum = freeinode++;
  struct dinode din;

  bzero(&din, sizeof(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.size = xint(0);
  winode(inum, &din);
  return inum;
}

void
balloc(int used)
{
  uchar buf[BSIZE];
  int i;

  printf("balloc: first %d blocks have been allocated\n", used);
  assert(used < BSIZE*8);
  bzero(buf, BSIZE);
  for(i = 0; i < used; i++){
    buf[i/8] = buf[i/8] | (0x1 << (i%8));
  }
  printf("balloc: write bitmap block at sector %d\n", sb.bmapstart);
  wsect(sb.bmapstart, buf);
}

void
iappend(uint inum, void *xp, int n)
{
  char *p = (char*)xp;
  uint fbn, off, n1;
  struct dinode din;
  char buf[BSIZE];
  uint indirect[NINDIRECT];
  uint x;

  rinode(inum, &din);
  off = xint(din.size);
  // printf("append inum %d at off %d sz %d\n", inum, off, n);
  while(n > 0){
    fbn = off / BSIZE;
    assert(fbn < MAXFILE);
    if(fbn < NDIRECT){
      if(xint(din.addrs[fbn]) == 0){
        din.addrs[fbn] = xint(freeblock++);
      }
      x = xint(din.addrs[fbn]);
    } else {
      if(xint(din.addrs[NDIRECT]) == 0){
        din.addrs[NDIRECT] = xint(freeblock++);
      }
      rsect(xint(din.addrs[NDIRECT]), (char*)indirect);
      if(indirect[fbn - NDIRECT] == 0){
        indirect[fbn - NDIRECT] = xint(freeblock++);
        wsect(xint(din.addrs[NDIRECT]), (char*)indirect);
      }
      x = xint(indirect[fbn-NDIRECT]);
    }
    n1 = min(n, (fbn + 1) * BSIZE - off);
    rsect(x, buf);
    bcopy(p, buf + off - (fbn * BSIZE), n1);
    wsect(x, buf);
    n -= n1;
    off += n1;
    p += n1;
  }
  din.size = xint(off);
  winode(inum, &din);
}
