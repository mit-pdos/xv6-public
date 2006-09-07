struct stat {
  int dev;     // Device number
  uint ino;    // Inode number on device
  short type;  // Type of file
  short nlink; // Number of links to file
  uint size;   // Size of file in bytes
};
