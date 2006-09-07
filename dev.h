struct devsw {
  int (*read)(int, char*, int);
  int (*write)(int, char*, int);
};

extern struct devsw devsw[];

#define CONSOLE 1
