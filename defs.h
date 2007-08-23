struct buf;
struct file;
struct inode;
struct jmpbuf;
struct pipe;
struct proc;
struct spinlock;
struct stat;
struct uinode;

// 8253pit.c
void            pit8253_timerinit(void);

// bio.c
void            binit(void);
struct buf*     bread(uint, uint);
void            brelse(struct buf*);
void            bwrite(struct buf*);

// console.c
void            console_init(void);
void            cprintf(char*, ...);
void            kbd_intr(void);
void            panic(char*) __attribute__((noreturn));

// exec.c
int            exec(char*, char**);

// file.c
struct file*    filealloc(void);
void            fileclose(struct file*);
void            fileincref(struct file*);
void            fileinit(void);
int             fileread(struct file*, char*, int n);
int             filestat(struct file*, struct stat*);
int             filewrite(struct file*, char*, int n);

// fs.c
int             dirlink(struct inode*, char*, uint);
struct uinode*  dirlookup(struct inode*, char*, uint*);
struct uinode*  ialloc(uint, short);
struct uinode*  idup(struct uinode*);
void            iinit(void);
struct inode*   ilock(struct uinode*);
struct uinode*  iunlock(struct inode*);
void            iput(struct uinode*);
void            iupdate(struct inode*);
int             namecmp(const char*, const char*);
struct uinode*  namei(char*);
struct uinode*  nameiparent(char*, char*);
int             readi(struct inode*, char*, uint, uint);
void            stati(struct inode*, struct stat*);
int             writei(struct inode*, char*, uint, uint);

// ide.c
void            ide_init(void);
void            ide_intr(void);
void            ide_rw(int, uint, void*, uint, int);

// ioapic.c
void            ioapic_enable(int irq, int cpu);
extern uchar    ioapic_id;
void            ioapic_init(void);

// kalloc.c
char*           kalloc(int);
void            kfree(char*, int);
void            kinit(void);

// lapic.c
int             cpu(void);
extern uint*    lapicaddr;
void            lapic_disableintr(void);
void            lapic_enableintr(void);
void            lapic_eoi(void);
void            lapic_init(int);
void            lapic_startap(uchar, int);
void            lapic_timerinit(void);
void            lapic_timerintr(void);

// mp.c
extern int      ismp;
int             mp_bcpu(void);
void            mp_init(void);
void            mp_startthem(void);

// picirq.c
void            irq_enable(int);
void            pic_init(void);

// pipe.c
int             pipe_alloc(struct file**, struct file**);
void            pipe_close(struct pipe*, int);
int             pipe_read(struct pipe*, char*, int);
int             pipe_write(struct pipe*, char*, int);

// proc.c
struct proc*    copyproc(struct proc*);
int             growproc(int);
void            pinit(void);
void            proc_exit(void);
int             proc_kill(int);
int             proc_wait(void);
void            procdump(void);
void            scheduler(void);
void            setupsegs(struct proc*);
void            sleep(void*, struct spinlock*);
void            userinit(void);
void            wakeup(void*);
void            yield(void);

// setjmp.S
void            longjmp(struct jmpbuf*);
int             setjmp(struct jmpbuf*);

// spinlock.c
void            acquire(struct spinlock*);
void            getcallerpcs(void*, uint*);
int             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);

// string.c
int             memcmp(const void*, const void*, uint);
void*           memmove(void*, const void*, uint);
void*           memset(void*, int, uint);
char*           safestrcpy(char*, const char*, int);
int             strlen(const char*);
int             strncmp(const char*, const char*, uint);

// syscall.c
int             argint(int, int*);
int             argptr(int, char**, int);
int             argstr(int, char**);
int             fetchint(struct proc*, uint, int*);
int             fetchstr(struct proc*, uint, char**);
void            syscall(void);

// trap.c
void            idtinit(void);
void            tvinit(void);

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x)/sizeof((x)[0]))

