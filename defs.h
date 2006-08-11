// kalloc.c
char *kalloc(int n);
void kfree(char *cp, int len);
void kinit(void);

// console.c
void console_init(void);
void cprintf(char *fmt, ...);
void panic(char *s);
void kbd_intr(void);

// proc.c
void pinit(void);
struct proc;
struct jmpbuf;
void setupsegs(struct proc *);
struct proc * copyproc(struct proc*);
struct spinlock;
void sleep(void *, struct spinlock *);
void wakeup(void *);
void scheduler(void);
void proc_exit(void);
int proc_kill(int);
int proc_wait(void);
void yield(void);

// swtch.S
struct jmpbuf;
int setjmp(struct jmpbuf*);
void longjmp(struct jmpbuf*);

// trap.c
void tvinit(void);
void idtinit(void);

// string.c
void * memset(void *dst, int c, uint n);
int memcmp(const void *v1, const void *v2, uint n);
void *memmove(void *dst, const void *src, uint n);
int strncmp(const char *p, const char *q, uint n);

// syscall.c
void syscall(void);

// picirq.c
void pic_init(void);

// mp.c
void mp_init(void);
void mp_startthem(void);
int mp_bcpu(void);

// lapic
extern uint *lapicaddr;
void lapic_init(int);
void lapic_startap(uchar, int);
void lapic_timerinit(void);
void lapic_timerintr(void);
void lapic_enableintr(void);
void lapic_disableintr(void);
void lapic_eoi(void);
int cpu(void);

// ioapic
extern uchar ioapic_id;
void ioapic_init(void);
void ioapic_enable (int irq, int cpu);

// spinlock.c
struct spinlock;
void initlock(struct spinlock *, char *);
void acquire(struct spinlock*);
void release(struct spinlock*);
int holding(struct spinlock*);

// main.c
void load_icode(struct proc *p, uchar *binary, uint size);

// pipe.c
struct pipe;
struct fd;
int pipe_alloc(struct fd **fd1, struct fd **fd2);
void pipe_close(struct pipe *p, int writeable);
int pipe_write(struct pipe *p, char *addr, int n);
int pipe_read(struct pipe *p, char *addr, int n);

// fd.c
void fd_init(void);
int fd_ualloc(void);
struct fd * fd_alloc(void);
void fd_close(struct fd *);
int fd_read(struct fd *fd, char *addr, int n);
int fd_write(struct fd *fd, char *addr, int n);
void fd_incref(struct fd *fd);

// ide.c
void ide_init(void);
void ide_intr(void);
void* ide_start_rw(int diskno, uint secno, void *dst, uint nsecs, int read);
int ide_finish(void *);

// bio.c
void binit(void);
struct buf;
struct buf *getblk(void);
struct buf *bread(uint, uint);
void bwrite(uint, struct buf *, uint);
void brelse(struct buf *);

// fs.c
void iinit(void);
struct inode * iget(uint dev, uint inum);
void ilock(struct inode *ip);
void iunlock(struct inode *ip);
void idecref(struct inode *ip);
void iput(struct inode *ip);
struct inode * namei(char *path);
int readi(struct inode *ip, char *xdst, uint off, uint n);
int writei(struct inode *ip, char *addr, uint off, uint n);
struct inode *mknod(struct inode *, char *, short, short, short);
void iupdate (struct inode *ip);
