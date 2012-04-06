#include <stdarg.h>

BEGIN_DECLS
struct stat;
struct ipcmsg;

// system calls
int fork(int);
int exit(void) __attribute__((noreturn));
int wait(void);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(const char*, const char**);
int openat(int dirfd, const char *pathname, int omode);
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdir(const char*);
int mkdirat(int dirfd, const char *pathname);
int chdir(const char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int nsleep(u64);
u64 uptime(void);
int map(void *addr, size_t len);
int unmap(void *addr, size_t len);
int halt(void);
ssize_t pread(int, void*, size_t, off_t);
int async(int, size_t, off_t, u32, u32);
int script(void *addr, u64 len, u64 chunk);
int setfs(u64 base);
int setaffinity(int cpu);
long futex(const u64* addr, int op, u64 val, u64 timer);

// ulib.c
int stat(char*, struct stat*);
int fstatat(int dirfd, const char*, struct stat*);
char* strcpy(char*, const char*);
char* strncpy(char *s, const char *t, size_t n);
void* memcpy(void *dst, const void *src, size_t n);
void* memmove(void*, const void*, size_t);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
int strncmp(const char *p, const char *q, size_t n);
int open(const char*, int);

char* gets(char*, int max);
unsigned int strlen(const char*);
void* memset(void*, int, unsigned int);
void* malloc(unsigned int);
void free(void*);
int atoi(const char*);

// uthread.S
int forkt(void *sp, void *pc, void *arg, int forkflags);
void forkt_setup(u64 pid);

// printf.c
void printf(const char*, ...);
void fprintf(int, const char*, ...);
void snprintf(char *buf, unsigned int n, const char *fmt, ...);
void vsnprintf(char *buf, u32 n, const char *fmt, va_list ap);
void die(const char* errstr, ...) __attribute__((noreturn));
#define assert(c) if (!(c)) { fprintf(2, "%s:%d: ", __FILE__, __LINE__); die("assertion failure"); }

// perf.cc
// Default selector for AMD 10h:
//  [35 - 32]   event mask [11 - 8]
//  [31 - 24]   counter mask
//  [22]        counter enable
//  [17]        operating system mode
//  [16]        user mode
//  [15 - 8]    unit mask
//  [7 - 0]     event mask [7 - 0]
#define PERF_SELECTOR \
  (0UL<<32 | 1<<24 | 1<<22 | 1<<20 | 1<<17 | 1<<16 | 0x00<<8 | 0x76)
// Default period
#define PERF_PERIOD 100000
void perf_stop(void);
void perf_start(u64 selector, u64 period);
END_DECLS
