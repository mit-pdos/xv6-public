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
int chdir(const char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
int map(void *addr, int len);
int unmap(void *addr, int len);
void halt(void);
ssize_t pread(int, void*, size_t, off_t);
int async(int, size_t, off_t, u32, u32);
int script(void *addr, u64 len, u64 chunk);
int setfs(u64 base);

// ulib.c
int stat(char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
int strncmp(const char *p, const char *q, u32 n);
int open(const char*, int);

char* gets(char*, int max);
unsigned int strlen(const char*);
void* memset(void*, int, unsigned int);
void* malloc(unsigned int);
void free(void*);
int atoi(const char*);

// uthread.S
int forkt(void *sp, void *pc, void *arg);
void forkt_setup(u64 pid);

// printf.c
void printf(const char*, ...);
void fprintf(int, const char*, ...);
void snprintf(char *buf, unsigned int n, const char *fmt, ...);
void die(const char* errstr, ...) __attribute__((noreturn));
#define assert(c) if (!(c)) { fprintf(2, "%s:%d: ", __FILE__, __LINE__); die("assertion failure"); }
END_DECLS
