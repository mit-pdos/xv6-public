typedef unsigned short uint16;
typedef unsigned int uint32;
typedef long int64;
typedef unsigned long uint64;
typedef unsigned char uint8;
typedef uint64 uintptr;

struct Mbdata;
void	bootpanic(char*);
void	initcga(void);
void	initmem(struct Mbdata*);
void	initpic(void);
void	inittrap(void);
void	initlapic(int);
int cpunum(void);
void	lapiceoi(void);
void	lapicstartap(uint8, uint32);
uint8	inb(uint16);
void	outb(uint16, uint8);
void	microdelay(int);
uint32	readeflags(void);
void	setcursor(int);
void	memmove(void *dst, void *src, int64 n);
int memcmp(const void*, const void*, int64);
void	hlt(void);
void	panic(const char*);
void	puts(const char*);
void	putsn(const char*, int);
int strlen(const char*);
void	printf(const char*, ...);
void	lidt(void*, int);
void	ltr(int);
void	lgdt(void*, int);

#define KBASE 0xffffff0000000000ull
#define KADDR(x) ((void*)(KBASE+(uintptr)(x)))
#define PADDR(x) ((uintptr)(x) - KBASE)
#define PGSIZE (2*1024*1024ull)

#define KCSEG (2<<3)  /* kernel code segment */
#define KDSEG (3<<3)  /* kernel data segment */

#define nil ((void*)0)
