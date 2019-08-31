#define O_RDONLY  0x000
#define O_WRONLY  0x001
#define O_RDWR    0x002
#define O_CREATE  0x200


//ioctl tty command types
#define DEV_CONNECT     0x1000
#define DEV_DISCONNECT  0x2000
#define DEV_ATTACH      0x0010
#define DEV_DETACH      0x0020

//ioctl tty requests types
#define TTYSETS  0x0001
#define TTYGETS  0x0002

