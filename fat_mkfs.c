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
#define FNSIZE 14

typedef struct direntry DIR

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
uint fatFstSec = nDBR + nRetain;  // FAT表起始扇区号
uint fatTmpFstSec = fatFstSec + nFAT; // FAT备份表起始扇区号
uint freeClusIdx = 2; // FAT表中空闲位置的index(以4字节为单位)
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
* 参数说明：clus簇号,offsec簇中扇区偏移量[0,1...SECPERCLUS),buf数据,curWSize为已向该簇中写入的字节数
*/
void appendBuf(uint clus, void *buf, int size, int curWSize);
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
// 
void wsect4bytes(uint sec, uint index, void *buf);
void rsect4bytes(uint sec, uint index, void *buf);
void wsectnbytes(uint sec, uint index, void *buf, int wn);
struct direntry mkFCB(uchar type, uchar *name, int size, uint *clusNum);
uint retFileSize(uchar* filename);
// convert to intel byte order
ushort xshort(ushort x);
uint xint(uint x);


int main(int argc, char *argv[])
{
  // 局部变量
  uchar buf[SECSIZE];
//  uchar fatStartContent[8] = {0xf8,0xff,0xff,0x0f,0xff,0xff,0xff,0xff};
  struct direntry dire;
  uint dirClusNum;  // 为文件分配的簇号
  uint wClusNum;  // 根目录簇号
  uint fileSize;  // 文件大小
  uint cc, fd;  // cc一次读取文件的字节数，打开文件的fd
  int rootWSize = 0;  // 已向根目录写入的字节数
  int fileWSize = 0;  // 已向文件中写入的字节数
  uchar filename[FNSIZE]; // 文件名

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

  // 初始化DBR,FSI
  initDBR();
  initFSI();
  printf("DBR : %d, retain sectors: %d, FAT sector: %d, 2 FATs, data sectors: %d, clusters: %d", 
    nDBR, nRetain, nFAT, nData, nDataClus);

  // 清零fs
  for(i = 0; i < FSSIZE; i++)
    szero(i);

  // 写入DBR,FSI
  memset(buf, 0, sizeof(buf));
  memmove(buf, &fatDBR, sizeof(fatDBR));
  wsect(0, buf);
  memset(buf, 0, sizeof(buf));
  memmove(buf, &fsi, sizeof(fsi));
  wsect(1, buf);

  // // 写入FAT表头标记
  // rsect4bytes(fatFstSec, 0, fatStartContent);
  // rsect4bytes(fatFstSec, 1, fatStartContent + 4);

  // 写入根目录
  dire = mkFCB(T_DIR, "/", sizeof(DIR)*argc, &dirClusNum);
  appendBuf(dirClusNum, &dire, sizeof(DIR), rootWSize);
  rootWSize += sizeof(DIR);
  wClusNum = dirClusNum;

  dire = mkFCB(T_DIR, ".", sizeof(DIR)*argc, &dirClusNum);
  appendBuf(wClusNum, &dire, sizeof(DIR), rootWSize);
  rootWSize += sizeof(DIR);

  dire = mkFCB(T_DIR, "..", 0, &dirClusNum);
  appendBuf(wClusNum, &dire, sizeof(DIR), rootWSize);
  rootWSize += sizeof(DIR);

  // 写入其他文件
  for(i = 2; i < argc; i++){
    fileWSize = 0;
    assert(index(argv[i], '/') == 0);

    fileSize = retFileSize(argv[i]);

    if((fd = open(argv[i], 0)) < 0){
      perror(argv[i]);
      exit(1);
    }
    
    if(argv[i][0] == '_')
      ++argv[i];

    // 写到目录区
    strncpy(filename, argv[i], FNSIZE);
    dire = mkFCB(T_FILE, filename, fileSize, &dirClusNum);
    appendBuf(wClusNum, &dire, sizeof(DIR), rootWSize);
    rootWSize += sizeof(DIR);

    // 写入文件
    appendBuf(dirClusNum, &dire, sizeof(DIR), fileWSize);
    fileWSize += sizeof(DIR);
    while((cc = read(fd, buf, sizeof(buf))) > 0)
    {
      appendBuf(dirClusNum, buf, cc, fileWSize);
      fileWSize += cc;
    }

    close(fd);
  }

  // // fix size of root inode dir
  // rinode(rootino, &din);
  // off = xint(din.size);
  // off = ((off/BSIZE) + 1) * BSIZE;
  // din.size = xint(off);
  // winode(rootino, &din);

  // balloc(freeblock);

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

uint retFileSize(uchar* filename)
{
  uint fd;
  uint fs = 0;
  uint cc = 0;
  uchar buf[SECSIZE];
  if((fd = open(filename, , 0)) < 0) {
    return -1;
  }
  while((cc = read(fd, buf, sizeof(buf))) > 0) {
    fs += cc;
  }
  close(fd);
  return fs;
}

struct direntry mkFCB(uchar type, uchar *name, int size, uint *clusNum)
{
  char buf[4] = {0xff,0xff,0xff,0xff};
  struct direntry de;
  strncpy(de.deName, name, 8);
  memset(de.deExtension, 0, sizeof(de.deExtension));
  de.deAttributes = type;
  // 文件创建时间和日期
  // ?
  de.deFileSize = size;
  *clusNum = cnallloc();
  de.deHighClust = (ushort)((*clusNum) >> 16);
  de.deLowCluster = (ushort)(*clusNum);
  rsect4bytes(fatFstSec, *clusNum, buf);
  return de;
}

void wsect4bytes(uint sec, uint index, void *buf)
{
  uint offset = sec * SECSIZE + index * 4;
  if(lseek(fsfd, offset, 0) != offset) {
    perror("lseek.");
    exit(1);
  }
  if(write(fsfd, buf, 4) != 4) {
    perror("write4Bytes.");
    exit(1);
  }
}

void rsect4bytes(uint sec, uint index, void *buf)
{
  uint offset = sec * SECSIZE + index * 4;
  if (lseek(fsfd, offset, 0) != offset) {
    perror("seek.");
    exit(1);
  }
  if (read(fsfd, buf, 4) != 4) {
    perror("rsect4bytes.");
    exit(1);
  }
}

uint cnallloc()
{
  uint sect;
  char buf[SECSIZE];
  int i, temp;
  while(freeClusNum < nDataClus) {
    temp = 0;
    sect = clus2sec(freeClusNum);
    rsect(sect, buf);
    for(i = 0; i < SECSIZE; i++) {
      if(buf[i] != 0) {
        temp = 1;
        break;
      }
    }
    freeClusNum++;
    if (temp = 0) {
      return freeClusNum;
    }
  }
  return -1;
}

uint fatidxalloc()
{
  uint fatStart = fatFstSec;
  uint numOf4Bytes = (SECSIZE * nFAT / 4 - 8);
  int i, j;
  char buf[4];
  rsect4bytes(fatStart, freeClusIdx, buf);
  if(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0) {
    return freeClusIdx++;
  }
  while(freeClusIdx < numOf4Bytes) {
    rsect4bytes(fatStart, freeClusIdx, buf);
    freeClusIdx++;
    if(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0) {
      return freeClusIdx;
    }
  }
  freeClusIdx = 0;
  while(freeClusIdx < numOf4Bytes) {
    rsect4bytes(fatStart, freeClusIdx, buf);
    freeClusIdx++;
    if(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0) {
      return freeClusIdx;
    }
  }
  return -1;
}

void wsectnbytes(uint sec, uint index, void *buf, int wn)
{
  uint offset = sec * SECSIZE + index;
  if(lseek(fsfd, offset, 0) != offset) {
    perror("lseek.");
    exit(1);
  }
  if(write(fsfd, buf, wn) != wn) {
    perror("write4Bytes.");
    exit(1);
  }
}

void appendBuf(uint clus, void *buf, int size, int curWSize)
{
  uint expandClusNo = -1; // 下一个簇号
  uchar nextClus[4];  // 下一个簇号
  uint clusBytes = SECPERCLUS * SECSIZE;  // 单个簇字节数
  uint curClus = clus + (curWSize / clusBytes); // 需要向curClus簇内写入内容
  uint offSec = (curWSize % clusBytes) / SECSIZE; // 取值范围：[0,1...SECPERCLUS)
  uint curSec = clus2sec(curClus) + offSec;
  uint hasWBytes = (curWSize % clusBytes) % SECSIZE;
  uint leaveBytes = SECSIZE - hasWBytes; // 该扇区剩余的空闲字节数
  // priority 1: 是否需要扩展簇？
  if(size + (curWSize % clusBytes) > clusBytes) {
    expandClusNo = cnallloc();
    nextClus = (uchar*)(&expandClusNo);
    nextClus[0] = expandClusNo;
    nextClus[1] = expandClusNo >> 8;
    nextClus[2] = expandClusNo >> 16;
    nextClus[3] = expandClusNo >> 24;
    rsect4bytes(fatFstSec, curClus, nextClus);
  }
  // 向簇中写入buf
  while(size > 0) {
    wsectnbytes(curSec, hasWBytes, buf, leaveBytes);
    buf += leaveBytes;
    size -= leaveBytes;
    curSec = (offSec + 1 >= SECPERCLUS) ? (clus2sec(expandClusNo)) : (curSec + 1);
    offSec = (offSec + 1 >= SECPERCLUS) ? (0) : (offSec + 1);
    hasWBytes = 0;
    leaveBytes = SECPERCLUS;
  }
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


/*
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
*/