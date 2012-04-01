#pragma once

extern "C" {
#include "kern_c.h"
}

#include "atomic.hh"
#include "memlayout.h"
#include <stdarg.h>

#define KCSEG (2<<3)  /* kernel code segment */
#define KDSEG (3<<3)  /* kernel data segment */

static inline uptr v2p(void *a) {
  uptr ua = (uptr) a;
  if (ua >= KCODE)
    return ua - KCODE;
  else
    return ua - KBASE;
}

static inline void *p2v(uptr a) {
  uptr ac = a + KCODE;
  if (ac >= KCODE)
    return (void*) ac;
  else
    return (u8 *) a + KBASE;
}

struct trapframe;
struct spinlock;
struct condvar;
struct context;
struct vmnode;
struct inode;
struct node;
struct file;
struct stat;
struct proc;
struct vmap;
struct pipe;
struct work;

// bio.c
void            binit(void);
struct buf*     bread(u32, u64, int writer);
void            brelse(struct buf*, int writer);
void            bwrite(struct buf*);

// cga.c
void            cgaputc(int c);

// condvar.c
extern u64 ticks;
extern struct spinlock tickslock;
extern struct condvar cv_ticks;
void            initcondvar(struct condvar *, const char *);
void            destroycondvar(struct condvar *);
void            cv_sleep(struct condvar *cv, struct spinlock*);
void            cv_sleepto(struct condvar *cv, struct spinlock*, u64);
void            cv_wakeup(struct condvar *cv);
void            cv_tick(void);

// console.c
void            cprintf(const char*, ...) __attribute__((format(printf, 1, 2)));
void            vcprintf(const char *fmt, va_list ap);
void            panic(const char*, ...) 
                  __noret__ __attribute__((format(printf, 1, 2)));
void            kerneltrap(struct trapframe *tf) __noret__;
void            vsnprintf(char *buf, u32 n, const char *fmt, va_list ap);
void            snprintf(char *buf, u32 n, const char *fmt, ...);
void            printtrace(u64 rbp);

#define assert(c)   if (!(c)) { cprintf("%s:%d: ", __FILE__, __LINE__); panic("assertion failure"); }

// e1000.c
extern int e1000irq;
extern int e1000init;
void            e1000intr(void);
int             e1000tx(void *buf, u32 len);
void            e1000hwaddr(u8 *hwaddr);

// exec.c
int             exec(const char*, char**, void* ascope);

// fs.c
int             namecmp(const char*, const char*);
struct inode*   dirlookup(struct inode*, char*);
struct inode*   ialloc(u32, short);
struct inode*   namei(inode *cwd, const char*);
void            iput(struct inode*);
struct inode*   iget(u32 dev, u32 inum);
struct inode*   igetnoref(u32 dev, u32 inum);
void            ilock(struct inode*, int writer);
void            iunlockput(struct inode*);
void            iupdate(struct inode*);
void            iunlock(struct inode*);
int             readi(struct inode*, char*, u32, u32);
void            stati(struct inode*, struct stat*);
int             writei(struct inode*, char*, u32, u32);
struct inode*   idup(struct inode*);
struct inode*   nameiparent(inode *cwd, const char*, char*);
int             dirlink(struct inode*, const char*, u32);
void            dir_init(struct inode *dp);
void	        dir_flush(struct inode *dp);

// hz.c
void            microdelay(u64);
u64             nsectime(void);
void            inithz(void);

// ide.c
void            ideinit(void);
void            ideintr(void);
void            iderw(struct buf*);

// idle.cc
struct proc *   idleproc(void);
void            idlezombie(struct proc*);

// ioapic.c
void            ioapicenable(int irq, int cpu);

// kalloc.c
char*           kalloc(const char *name);
void            kfree(void*);
void*           ksalloc(int slabtype);
void            ksfree(int slabtype, void*);
void*           kmalloc(u64 nbytes, const char *name);
void            kmfree(void*, u64 nbytes);
int             kmalign(void **p, int align, u64 size, const char *name);
void            kmalignfree(void *, int align, u64 size);
void            verifyfree(char *ptr, u64 nbytes);
void            kminit(void);
void            kmemprint(void);

// kbd.c
void            kbdintr(void);

// lapic.c
void            lapicstartap(hwid_t, u32 addr);
void            lapiceoi(void);
void            lapic_tlbflush(hwid_t);
void            lapic_sampconf(hwid_t);
void            lapicpc(char mask);

// mp.c
extern int      ncpu;
int             mpbcpu(void);

// net.c
void            netfree(void *va);
void*           netalloc(void);
void            netrx(void *va, u16 len);
int             nettx(void *va, u16 len);
void            nethwaddr(u8 *hwaddr);

// picirq.c
void            picenable(int);
void            piceoi(void);

// pipe.c
int             pipealloc(struct file**, struct file**);
void            pipeclose(struct pipe*, int);
int             piperead(struct pipe*, char*, int);
int             pipewrite(struct pipe*, char*, int);

// proc.c
struct proc*    copyproc(struct proc*);
void            finishproc(struct proc*);
void            exit(void);
int             fork(int);
int             growproc(int);
void            pinit(void);
void            procdumpall(void);
void            scheduler(void) __noret__;
void            userinit(void);
int             wait(void);
void            yield(void);
struct proc*    threadalloc(void (*fn)(void*), void *arg);
struct proc*    threadpin(void (*fn)(void*), void *arg, const char *name, int cpu);

// sampler.c
void            sampstart(void);
int             sampintr(struct trapframe*);
void            sampdump(void);
void            sampconf(void);
void            sampidle(bool);

// sched.cc
void            addrun(struct proc *);
void            sched(void);
void            post_swtch(void);
void            scheddump(void);

// spinlock.c
void            acquire(struct spinlock*);
int             tryacquire(struct spinlock*);
int             holding(struct spinlock*);
void            initlock(struct spinlock*, const char*, int);
void            destroylock(struct spinlock *lk);
void            release(struct spinlock*);

// syscall.c
int             argcheckptr(void *argval, int);
int             argcheckstr(const char*);
int             fetchint64(uptr, u64*);
int             fetchstr(char*, const char*, u64);
int             fetchmem(void*, const void*, u64);
int             putmem(void*, const void*, u64);
u64             syscall(u64 a0, u64 a1, u64 a2, u64 a3, u64 a4, u64 num);

// string.c
extern  "C" int  memcmp(const void*, const void*, size_t);
void*           memmove(void*, const void*, size_t);
extern "C" void* memset(void*, int, size_t);
extern "C" void* memcpy(void*, const void *, size_t);
char*           safestrcpy(char*, const char*, size_t);
extern "C" unsigned int strlen(const char*);
int             strncmp(const char*, const char*, size_t);
char*           strncpy(char*, const char*, size_t);
int             strcmp(const char *p, const char *q);

// swtch.S
void            swtch(struct context**, struct context*);

// trap.c
extern struct segdesc bootgdt[NSEGS];
void            pushcli(void);
void            popcli(void);
void            getcallerpcs(void*, uptr*, int);

// uart.c
void            uartputc(char c);
void            uartintr(void);

// vm.c
void            switchvm(struct proc*);
int             pagefault(struct vmap *, uptr, u32);

// zalloc.cc
char*           zalloc(const char* name);
void            zfree(char* p);

// other exported/imported functions
void cmain(u64 mbmagic, u64 mbaddr);
void mpboot(void);
void trapret(void);
void threadstub(void);
void threadhelper(void (*fn)(void *), void *arg);
void trap(struct trapframe *tf);

