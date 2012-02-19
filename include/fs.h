// On-disk file system format. 
// Both the kernel and user programs use this header file.

// Block 0 is unused.
// Block 1 is super block.
// Inodes start at block 2.

#define ROOTINO 1  // root i-number
#define BSIZE 512  // block size

// File system super block
struct superblock {
  u32 size;         // Size of file system image (blocks)
  u32 nblocks;      // Number of data blocks
  u32 ninodes;      // Number of inodes.
};

#define NDIRECT 11
#define NINDIRECT (BSIZE / sizeof(u32))
#define MAXFILE (NDIRECT + NINDIRECT)

// On-disk inode structure
// (512 % sizeof(dinode)) == 0
struct dinode {
  short type;           // File type
  short major;          // Major device number (T_DEV only)
  short minor;          // Minor device number (T_DEV only)
  short nlink;          // Number of links to inode in file system
  u32 size;             // Size of file (bytes)
  u32 gen;              // Generation # (to check name cache)
  u32 addrs[NDIRECT+1]; // Data block addresses
};

// Inodes per block.
#define IPB           (BSIZE / sizeof(struct dinode))

// Block containing inode i
#define IBLOCK(i)     ((i) / IPB + 2)

// Bitmap bits per block
#define BPB           (BSIZE*8)

// Block containing bit for block b
#define BBLOCK(b, ninodes) (b/BPB + (ninodes)/IPB + 3)

// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ 14

struct dirent {
  u16 inum;
  char name[DIRSIZ];
};
