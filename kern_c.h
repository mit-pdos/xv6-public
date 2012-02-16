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
long sys_chdir(void);
long sys_close(void);
long sys_dup(void);
long sys_exec(void);
long sys_exit(void);
long sys_fork(void);
long sys_fstat(void);
long sys_getpid(void);
long sys_kill(void);
long sys_link(void);
long sys_mkdir(void);
long sys_mknod(void);
long sys_open(void);
long sys_pipe(void);
long sys_read(void);
long sys_sbrk(void);
long sys_sleep(void);
long sys_unlink(void);
long sys_wait(void);
long sys_write(void);
long sys_uptime(void);
long sys_map(void);
long sys_unmap(void);
long sys_halt(void);
long sys_socket(int, int, int);
long sys_bind(int, void*, int);
long sys_listen(int, int);
long sys_accept(int, void*, void*);
long sys_pread(int fd, void *ubuf, size_t count, off_t offset);
long sys_async(int, size_t, off_t, u32, u32);
extern long (*syscalls[])(u64, u64, u64, u64, u64, u64);

// other exported/imported functions
void cmain(u64 mbmagic, u64 mbaddr);
void mpboot(void);
void trapret(void);
void threadstub(void);
void threadhelper(void (*fn)(void *), void *arg);

struct trapframe;
void trap(struct trapframe *tf);

