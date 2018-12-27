#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <dirent.h>
#include <stdbool.h>

#define stat xv6_stat  // avoid clash with host struct stat
#define dirent xv6_dirent  // avoid clash with host struct stat
#include "types.h"
#include "fs.h"
#include "stat.h"
#undef stat
#undef dirent

#define BLOCK_SIZE (512)

int nblocks = 995;
int ninodes = 200;
int size = 1024;

int fsfd;
struct superblock sb;
char zeroes[512];
uint freeblock;
uint usedblocks;
uint bitblocks;
uint freeinode = 1;
uint root_inode;

void balloc(int);
void wsect(uint, void*);
void winode(uint, struct dinode*);
void rinode(uint inum, struct dinode *ip);
void rsect(uint sec, void *buf);
uint ialloc(ushort type);
void iappend(uint inum, void *p, int n);

// convert to intel byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar*)&y;
  a[0] = x;
  a[1] = x >> 8;
  return y;
}

uint
xint(uint x)
{
  uint y;
  uchar *a = (uchar*)&y;
  a[0] = x;
  a[1] = x >> 8;
  a[2] = x >> 16;
  a[3] = x >> 24;
  return y;
}


int 
mkfs(int nblocks, int ninodes, int size) {

  int i;
  char buf[BLOCK_SIZE];

  sb.size = xint(size);
  sb.nblocks = xint(nblocks); // so whole disk is size sectors
  sb.ninodes = xint(ninodes);

  bitblocks = size/(512*8) + 1;
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


  return 0;
}

int
add_dir(DIR *cur_dir, int cur_inode, int parent_inode) {
	int r;
	int child_inode;
	int cur_fd, child_fd;
	struct xv6_dirent de;
	struct dinode din;
	struct dirent dir_buf;
	struct dirent *entry;
	struct stat st;
	int bytes_read;
	char buf[BLOCK_SIZE];
	int off;

	bzero(&de, sizeof(de));
	de.inum = xshort(cur_inode);
	strcpy(de.name, ".");
	iappend(cur_inode, &de, sizeof(de));

	bzero(&de, sizeof(de));
	de.inum = xshort(parent_inode);
	strcpy(de.name, "..");
	iappend(cur_inode, &de, sizeof(de));

	if (cur_dir == NULL) {
		return 0;
	}

	cur_fd = dirfd(cur_dir);
	if (cur_fd == -1){
		perror("add_dir");
		exit(EXIT_FAILURE);
	}

	if (fchdir(cur_fd) != 0){
		perror("add_dir");
		return -1;
	}

	while (true) {
		r = readdir_r(cur_dir, &dir_buf, &entry);

		if (r != 0) {
			perror("add_dir");
			return -1;
		}

		if (entry == NULL)
			break;

		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;

		printf("%s\n", entry->d_name);

		child_fd = open(entry->d_name, O_RDONLY);
		if (child_fd == -1) {
			perror("open");
			return -1;
		}

		r = fstat(child_fd, &st);
		if (r != 0) {
			perror("stat");
			return -1;
		}

		if (S_ISDIR(st.st_mode)) {
      child_inode = ialloc(T_DIR);
			r = add_dir(fdopendir(child_fd), child_inode, cur_inode);
			if (r != 0) return r;
			if (fchdir(cur_fd) != 0) {
				perror("chdir");
				return -1;
			}
		} else {
			bytes_read = 0;
	  		child_inode = ialloc(T_FILE);
			bzero(&de, sizeof(de));
			while((bytes_read = read(child_fd, buf, sizeof(buf))) > 0) {
				iappend(child_inode, buf, bytes_read);
			}
		}
		close(child_fd);

		de.inum = xshort(child_inode);
		strncpy(de.name, entry->d_name, DIRSIZ);
		iappend(cur_inode, &de, sizeof(de));

	}

	// fix size of inode cur_dir
	rinode(cur_inode, &din);
	off = xint(din.size);
	off = ((off/BSIZE) + 1) * BSIZE;
	din.size = xint(off);
	winode(cur_inode, &din);
	return 0;
}




int
main(int argc, char *argv[])
{
  int r;
  DIR *root_dir;

  if(argc < 2){
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    exit(1);
  }

  assert((512 % sizeof(struct dinode)) == 0);
  assert((512 % sizeof(struct xv6_dirent)) == 0);

  fsfd = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, 0666);
  if(fsfd < 0){
    perror(argv[1]);
    exit(1);
  }

  mkfs(995, 200, 1024);

  root_dir = opendir(argv[2]);

  root_inode = ialloc(T_DIR);
  assert(root_inode == ROOTINO);

  r = add_dir(root_dir, root_inode, root_inode);
  if (r != 0) {
    exit(EXIT_FAILURE);
  }

  balloc(usedblocks);

  exit(0);
}

void
wsect(uint sec, void *buf)
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
  dip = ((struct dinode*)buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

void
rinode(uint inum, struct dinode *ip)
{
  char buf[512];
  uint bn;
  struct dinode *dip;

  bn = i2b(inum);
  rsect(bn, buf);
  dip = ((struct dinode*)buf) + (inum % IPB);
  *ip = *dip;
}

void
rsect(uint sec, void *buf)
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
  uchar buf[512];
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
iappend(uint inum, void *xp, int n)
{
  char *p = (char*)xp;
  uint fbn, off, n1;
  struct dinode din;
  char buf[512];
  uint indirect[NINDIRECT];
  uint x;

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
