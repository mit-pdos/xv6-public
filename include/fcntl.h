#define O_RDONLY  0x000
#define O_WRONLY  0x001
#define O_RDWR    0x002
#define O_CREATE  0x200
#define O_WAIT    0x400 // open waits for create, read for write

#define AT_FDCWD  -100

#define FORK_SHARE_VMAP     (1<<0)
#define FORK_SHARE_FD       (1<<1)
#define FORK_SEPARATE_PGMAP (1<<2)
