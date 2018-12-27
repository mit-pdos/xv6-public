#ifndef _STAT_H_
#define _STAT_H_

// Stat struct definition, for use with stat syscall

#define T_DIR  1   // Directory
#define T_FILE 2   // File
#define T_DEV  3   // Special device

struct stat {
  short type;  // Type of file
  int dev;     // Device number
  uint ino;    // Inode number on device
  short nlink; // Number of links to file
  uint size;   // Size of file in bytes
};

#endif // _STAT_H_
