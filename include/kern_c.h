#include "mmu.h"
#include "lib.h"

struct ipcmsg;

// console.c
void            consoleintr(int(*)(void));

// kbd.c
void            kbdintr(void);

// swtch.S
struct context;
void            swtch(struct context**, struct context*);

// trap.c
extern struct segdesc bootgdt[NSEGS];

// syscalls
struct stat;

long sys_chdir(const char*);
long sys_close(int);
long sys_dup(int);
long sys_exec(const char*, u64);
long sys_exit(void);
long sys_fork(int);
long sys_fstat(int, struct stat*);
long sys_getpid(void);
long sys_kill(int);
long sys_link(const char*, const char*);
long sys_mkdir(const char*);
long sys_mknod(const char*, int, int);
long sys_open(const char*, int);
long sys_pipe(int*);
long sys_read(int, char*, int);
long sys_sbrk(int);
long sys_sleep(int);
long sys_unlink(const char*);
long sys_wait(void);
long sys_write(int, char*, int);
long sys_uptime(void);
long sys_map(uptr, u64);
long sys_unmap(uptr, u64);
long sys_halt(void);
long sys_socket(int, int, int);
long sys_bind(int, void*, int);
long sys_listen(int, int);
long sys_accept(int, void*, void*);
long sys_pread(int fd, void *ubuf, size_t count, off_t offset);
long sys_async(int, size_t, off_t, u32, u32);
long sys_script(void *addr, u64 len, u64 chunk);
extern long (*syscalls[])(u64, u64, u64, u64, u64);

// other exported/imported functions
void cmain(u64 mbmagic, u64 mbaddr);
void mpboot(void);
void trapret(void);
void threadstub(void);
void threadhelper(void (*fn)(void *), void *arg);

struct trapframe;
void trap(struct trapframe *tf);
void sysentry(void);
u64 sysentry_c(u64 a0, u64 a1, u64 a2, u64 a3, u64 a4, u64 num);
