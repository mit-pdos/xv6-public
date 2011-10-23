// Memory layout

#define KSTKSIZE	(8*PGSIZE)   		// size of a kernel stack

#define IOSPACEB  0xA0000 // begin IO space
#define IOSPACEE  0x100000  // end IO space
#define PHYSTOP   0xE000000 // use phys mem up to here as free pool
