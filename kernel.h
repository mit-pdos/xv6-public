#define KBASE 0xFFFFFFFF80000000ull
#define PBASE 0xFFFFFF0000000000ull

#define KCSEG (2<<3)  /* kernel code segment */
#define KDSEG (3<<3)  /* kernel data segment */

#define NULL ((void *)0)

static inline uptr v2p(void *a) { return (uptr) a  - KBASE; }
static inline void *p2v(uptr a) { return (void *) a + KBASE; }

struct spinlock;

// console.c
void            cprintf(const char*, ...);
void            panic(const char*) __attribute__((noreturn));

// string.c
int             memcmp(const void*, const void*, u32);
void*           memmove(void*, const void*, u32);
void*           memset(void*, int, u32);
char*           safestrcpy(char*, const char*, int);
int             strlen(const char*);
int             strncmp(const char*, const char*, u32);
char*           strncpy(char*, const char*, int);
int             strcmp(const char *p, const char *q);

// spinlock.c
void            acquire(struct spinlock*);
void            getcallerpcs(void*, uptr*);
int             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            pushcli(void);
void            popcli(void);

// cga.c
void            cgaputc(char c);

// uart.c
void            uartputc(char c);

// mp.c
extern int      ncpu;
