#ifndef STAT_H_5F9EF308_CAAF_11EE_9131_90B11C0C0FF8
#define STAT_H_5F9EF308_CAAF_11EE_9131_90B11C0C0FF8
#define T_DIR  1   // Directory
#define T_FILE 2   // File
#define T_DEV  3   // Device

struct stat {
  short type;  // Type of file
  int dev;     // File system's disk device
  uint ino;    // Inode number
  short nlink; // Number of links to file
  uint size;   // Size of file in bytes
};
#endif
