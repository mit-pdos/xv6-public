// on-disk file system format

#define BSIZE 512  // block size

// sector 1 (2nd sector)
struct superblock{
  uint size;
  uint nblocks;
  uint ninodes;
};

#define NADDRS (NDIRECT+1)
#define NDIRECT 12
#define INDIRECT 12
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT  + NINDIRECT)

struct dinode {
  short type;
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NADDRS];
};

#define T_DIR 1
#define T_FILE 2
#define T_DEV 3

// sector 0 is unused, sector 1 is superblock, inodes start at sector 2
#define IPB (BSIZE / sizeof(struct dinode))
#define IBLOCK(inum) (inum / IPB + 2)   // start of inode
#define BPB (BSIZE*8)
#define BBLOCK(b,ninodes) (b/BPB + (ninodes/IPB) + 3)  // start of bitmap

#define DIRSIZ 14

struct dirent {
  ushort inum;
  char name[DIRSIZ];
};


