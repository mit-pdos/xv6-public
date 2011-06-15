#include <stdarg.h>

struct buf;
struct context;
struct file;
struct inode;
struct pipe;
struct proc;
struct spinlock;
struct condvar;
struct stat;
struct vmnode;
struct ns;

// bio.c
void            binit(void);
struct buf*     bread(uint, uint);
void            brelse(struct buf*);
void            bwrite(struct buf*);

// condvar.c
void            initcondvar(struct condvar *, char *);
void            cv_sleep(struct condvar *cv, struct spinlock*);
void            cv_wakeup(struct condvar *cv);

// console.c
void            consoleinit(void);
void            cprintf(char*, ...);
void            vsnprintf(char *buf, uint n, char *fmt, va_list ap);
void            snprintf(char *buf, uint n, char *fmt, ...);
void            consoleintr(int(*)(void));
void            panic(char*) __attribute__((noreturn));

// exec.c
int             exec(char*, char**);

// file.c
struct file*    filealloc(void);
void            fileclose(struct file*);
struct file*    filedup(struct file*);
void            fileinit(void);
int             fileread(struct file*, char*, int n);
int             filestat(struct file*, struct stat*);
int             filewrite(struct file*, char*, int n);

// fs.c
int             dirlink(struct inode*, char*, uint);
struct inode*   dirlookup(struct inode*, char*, uint*);
struct inode*   ialloc(uint, short);
struct inode*   iget(uint dev, uint inum);
struct inode*   idup(struct inode*);
void            iinit(void);
void            ilock(struct inode*, int writer);
void            iput(struct inode*);
void            iunlock(struct inode*);
void            iunlockput(struct inode*);
void            iupdate(struct inode*);
int             namecmp(const char*, const char*);
struct inode*   namei(char*);
struct inode*   nameiparent(char*, char*);
int             readi(struct inode*, char*, uint, uint);
void            stati(struct inode*, struct stat*);
int             writei(struct inode*, char*, uint, uint);

// ide.c
void            ideinit(void);
void            ideintr(void);
void            iderw(struct buf*);

// ioapic.c
void            ioapicenable(int irq, int cpu);
extern uchar    ioapicid;
void            ioapicinit(void);

// kalloc.c
char*           pgalloc(void);
void            kinit(void);
char*           kalloc(void);
void            kfree(char*);
void*           kmalloc(uint);
void            kmfree(void*);

// kbd.c
void            kbdintr(void);

// lapic.c
int             cpunum(void);
extern volatile uint*    lapic;
void            lapiceoi(void);
void            lapicinit(int);
void            lapic_tlbflush(uint);
void            lapicstartap(uchar, uint);
void            microdelay(int);

// mp.c
extern int      ismp;
int             mpbcpu(void);
void            mpinit(void);
void            mpstartthem(void);

// ns.c
void            nsinit(void);
struct ns*      nsalloc(int allowdup);
int             ns_allockey(struct ns*);
int             ns_insert(struct ns*, uint key, void*);
void*           ns_lookup(struct ns*, uint);
int             ns_remove(struct ns *ns, uint key, void *val);
void*           ns_enumerate(struct ns *ns, void *(*f)(uint, void *));
void*           ns_enumerate_key(struct ns *ns, uint key, void *(*f)(void *));


// picirq.c
void            picenable(int);
void            picinit(void);

// pipe.c
int             pipealloc(struct file**, struct file**);
void            pipeclose(struct pipe*, int);
int             piperead(struct pipe*, char*, int);
int             pipewrite(struct pipe*, char*, int);

//PAGEBREAK: 16
// proc.c
void            addrun(struct proc *);
struct proc*    copyproc(struct proc*);
void            exit(void);
int             fork(int);
int             growproc(int);
int             kill(int);
void            pinit(void);
void            procdumpall(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
void            userinit(void);
int             wait(void);
void            yield(void);
void            migrate(struct proc *);

// rcu.c
void            rcuinit(void);
void            rcu_begin_write(struct spinlock *);
void            rcu_end_write(struct spinlock *);
void            rcu_begin_read(void);
void            rcu_end_read(void);
void            rcu_delayed(void*, void (*dofree)(void*));
void		rcu_gc(void);

// swtch.S
void            swtch(struct context**, struct context*);

// spinlock.c
void            acquire(struct spinlock*);
void            getcallerpcs(void*, uint*);
int             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            pushcli(void);
void            popcli(void);

// string.c
int             memcmp(const void*, const void*, uint);
void*           memmove(void*, const void*, uint);
void*           memset(void*, int, uint);
char*           safestrcpy(char*, const char*, int);
int             strlen(const char*);
int             strncmp(const char*, const char*, uint);
char*           strncpy(char*, const char*, int);

// syscall.c
int             argint(int, int*);
int             argptr(int, char**, int);
int             argstr(int, char**);
int             fetchint(uint, int*);
int             fetchstr(uint, char**);
void            syscall(void);

// timer.c
void            timerinit(void);

// trap.c
void            idtinit(void);
extern uint     ticks;
void            tvinit(void);
extern struct spinlock tickslock;
extern struct condvar cv_ticks;

// uart.c
void            uartinit(void);
void            uartintr(void);
void            uartputc(int);

// vm.c
void            printpgdir(pde_t*);
void            pginit(char* (*alloc)());
void            seginit(void);
void            kvmalloc(void);
void            vmenable(void);
char*           uva2ka(pde_t*, char*);
struct vmnode*  vmn_alloc(uint, uint);
struct vmnode*  vmn_allocpg(uint);
void            vmn_free(struct vmnode *);
int             vmn_load(struct vmnode *, struct inode*, uint, uint);
struct vmap *   vmap_alloc(void);
void            vmap_decref(struct vmap *);
int             vmap_overlap(struct vmap *m, uint start, uint end);
int             vmap_insert(struct vmap *, struct vmnode *n, uint);
int             vmap_remove(struct vmap *, uint va_start, uint len);
struct vma *    vmap_lookup(struct vmap *, uint);
struct vmap *   vmap_copy(struct vmap *, int);
void            switchuvm(struct proc*);
void            switchkvm(void);
int             copyout(struct vmap *, uint, void*, uint);
int             copyin(struct vmap *, uint, void*, uint);
int             pagefault(struct vmap *, uint, uint);
void            clearpages(pde_t *pgdir, void *begin, void *end);

// namecache.c
void            nc_init();
struct inode *  nc_lookup(struct inode *, char *);
void            nc_insert(struct inode *, char *, struct inode *);
void            nc_invalidate(struct inode *dir, char *name);

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x)/sizeof((x)[0]))
#define NULL 0
