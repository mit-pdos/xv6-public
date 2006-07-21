// in-core file system types

struct inode {
  uint dev;
  uint inum;
  int count;
  int busy;
  short type;
  short nlink;
  uint size;
  uint addrs[NDIRECT];
};

extern uint rootdev;
