#define T_DIR  1   // Directory
#define T_FILE 2   // File
#define T_DEV  3   // Special device

struct stat {
  short type;  // Type of file
  int dev;     // Device number
  u32 ino;     // Inode number on device
  short nlink; // Number of links to file
  u32 size;    // Size of file in bytes
};
