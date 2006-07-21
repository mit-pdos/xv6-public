#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "types.h"
#include "param.h"
#include "fs.h"

int nblocks = 1009;
int ninodes = 100;

int fd;
struct superblock sb;
char zeroes[512];
uint freeblock;

void wsect(uint, void *);
void winode(uint, struct dinode *);
void rsect(uint sec, void *buf);

// convert to intel byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar *) &y;
  a[0] = x;
  a[1] = x >> 8;
  return y;
}

uint
xint(uint x)
{
  uint y;
  uchar *a = (uchar *) &y;
  a[0] = x;
  a[1] = x >> 8;
  a[2] = x >> 16;
  a[3] = x >> 24;
  return y;
}

main(int argc, char *argv[])
{
  int i;
  struct dinode din;
  char dbuf[512];
  uint bn;

  if(argc != 2){
    fprintf(stderr, "Usage: mkfs fs.img\n");
    exit(1);
  }

  if((512 % sizeof(struct dinode)) != 0){
    fprintf(stderr, "sizeof(dinode) must divide 512\n");
    exit(1);
  }
  if((512 % sizeof(struct dirent)) != 0){
    fprintf(stderr, "sizeof(dirent) must divide 512\n");
    exit(1);
  }

  fd = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, 0666);
  if(fd < 0){
    perror(argv[1]);
    exit(1);
  }

  sb.nblocks = xint(nblocks); // so whole disk is 1024 sectors
  sb.ninodes = xint(ninodes);

  freeblock = ninodes / IPB + 2;

  for(i = 0; i < nblocks + (ninodes / IPB) + 3; i++)
    wsect(i, zeroes);

  wsect(1, &sb);

  bzero(&din, sizeof(din));
  din.type = xshort(T_DIR);
  din.nlink = xshort(2);
  din.size = xint(512);
  bn = freeblock++;
  din.addrs[0] = xint(bn);
  winode(1, &din);

  bzero(dbuf, sizeof(dbuf));
  ((struct dirent *) dbuf)[0].inum = xshort(1);
  strcpy(((struct dirent *) dbuf)[0].name, ".");
  ((struct dirent *) dbuf)[1].inum = xshort(1);
  strcpy(((struct dirent *) dbuf)[1].name, "..");
  wsect(bn, dbuf);

  exit(0);
}

void
wsect(uint sec, void *buf)
{
  if(lseek(fd, sec * 512L, 0) != sec * 512L){
    perror("lseek");
    exit(1);
  }
  if(write(fd, buf, 512) != 512){
    perror("write");
    exit(1);
  }
}

uint
i2b(uint inum)
{
  return (inum / IPB) + 2;
}

void
winode(uint inum, struct dinode *ip)
{
  char buf[512];
  uint bn;
  struct dinode *dip;

  bn = i2b(inum);
  rsect(bn, buf);
  dip = ((struct dinode *) buf) + (inum % IPB);
  *dip = *ip;
  printf("bn %d off %d\n",
         bn, (unsigned)dip - (unsigned) buf);
  wsect(bn, buf);
}

void
rsect(uint sec, void *buf)
{
  if(lseek(fd, sec * 512L, 0) != sec * 512L){
    perror("lseek");
    exit(1);
  }
  if(read(fd, buf, 512) != 512){
    perror("read");
    exit(1);
  }
}
