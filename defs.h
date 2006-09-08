// kalloc.c
char* kalloc(int);
void kfree(char*, int);
void kinit(void);

// console.c
void console_init(void);
void cprintf(char*, ...);
void panic(char*);
void kbd_intr(void);

// proc.c
void pinit(void);
struct proc;
void setupsegs(struct proc*);
struct proc* copyproc(struct proc*);
struct spinlock;
int growproc(int);
void sleep(void*, struct spinlock*);
void wakeup(void*);
void scheduler(void);
void proc_exit(void);
int proc_kill(int);
int proc_wait(void);
void yield(void);
void procdump(void);

// setjmp.S
struct jmpbuf;
int setjmp(struct jmpbuf*);
void longjmp(struct jmpbuf*);

// trap.c
void tvinit(void);
void idtinit(void);

// string.c
void* memset(void*, int, uint);
int memcmp(const void*, const void*, uint);
void* memmove(void*, const void*, uint);
int strncmp(const char*, const char*, uint);

// syscall.c
void syscall(void);
int fetchint(struct proc*, uint, int*);
int fetchstr(struct proc*, uint, char**);
int argint(int, int*);
int argptr(int, char**, int);
int argstr(int, char**);

// picirq.c
void pic_init(void);
void irq_enable(int);

// 8253pit.c
void pit8253_timerinit(void);

// mp.c
extern int ismp;
void mp_init(void);
void mp_startthem(void);
int mp_bcpu(void);

// lapic.c
extern uint *lapicaddr;
void lapic_init(int);
void lapic_startap(uchar, int);
void lapic_timerinit(void);
void lapic_timerintr(void);
void lapic_enableintr(void);
void lapic_disableintr(void);
void lapic_eoi(void);
int cpu(void);

// ioapic.c
extern uchar ioapic_id;
void ioapic_init(void);
void ioapic_enable(int irq, int cpu);

// spinlock.c
struct spinlock;
void initlock(struct spinlock*, char*);
void acquire(struct spinlock*);
void release(struct spinlock*);
int holding(struct spinlock*);
void getcallerpcs(void*, uint*);

// main.c
void load_icode(struct proc*, uchar*, uint);

// pipe.c
struct pipe;
struct file;
int pipe_alloc(struct file**, struct file**);
void pipe_close(struct pipe*, int);
int pipe_write(struct pipe*, char*, int);
int pipe_read(struct pipe*, char*, int);

// file.c
struct stat;
void fileinit(void);
struct file* filealloc(void);
void fileclose(struct file*);
int fileread(struct file*, char*, int n);
int filewrite(struct file*, char*, int n);
int filestat(struct file*, struct stat*);
void fileincref(struct file*);

// ide.c
void ide_init(void);
void ide_intr(void);
void ide_rw(int, uint, void*, uint, int);

// bio.c
void binit(void);
struct buf;
struct buf* getblk(uint dev, uint sector);
struct buf* bread(uint, uint);
void bwrite(struct buf*, uint);
void brelse(struct buf*);

// fs.c
extern uint rootdev;
void iinit(void);
struct inode* iget(uint, uint);
void ilock(struct inode*);
void iunlock(struct inode*);
void itrunc(struct inode*);
void idecref(struct inode*);
void iincref(struct inode*);
void iput(struct inode*);
struct inode* namei(char*, int, uint*, char**, struct inode**);
void stati(struct inode*, struct stat*);
int readi(struct inode*, char*, uint, uint);
int writei(struct inode*, char*, uint, uint);
struct inode* mknod(char*, short, short, short);
struct inode* mknod1(struct inode*, char*, short, short, short);
int unlink(char*);
void iupdate(struct inode*);
int link(char*, char*);
