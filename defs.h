struct buf;
struct context;
struct file;
struct inode;
struct pipe;
struct proc;
struct spinlock;
struct stat;

// bio.c
void            binit(void);
struct buf*     bread(uint, uint);
void            brelse(struct buf*);
void            bwrite(struct buf*);

// console.c
void            console_init(void);
void            cprintf(char*, ...);
void            console_intr(int(*)(void));
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
struct inode*   idup(struct inode*);
void            iinit(void);
void            ilock(struct inode*);
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
void            ide_init(void);
void            ide_intr(void);
void            ide_rw(struct buf *);

// ioapic.c
void            ioapic_enable(int irq, int cpu);
extern uchar    ioapic_id;
void            ioapic_init(void);

// kalloc.c
char*           kalloc(int);
void            kfree(char*, int);
void            kinit(void);

// kbd.c
void            kbd_intr(void);

// lapic.c
int             cpu(void);
extern volatile uint*    lapic;
void            lapic_eoi(void);
void            lapic_init(int);
void            lapic_startap(uchar, uint);

// mp.c
extern int      ismp;
int             mp_bcpu(void);
void            mp_init(void);
void            mp_startthem(void);

// picirq.c
void            pic_enable(int);
void            pic_init(void);

// pipe.c
int             pipealloc(struct file**, struct file**);
void            pipeclose(struct pipe*, int);
int             piperead(struct pipe*, char*, int);
int             pipewrite(struct pipe*, char*, int);

// proc.c
struct proc*    copyproc(struct proc*);
struct proc*    curproc(void);
void            exit(void);
int             growproc(int);
int             kill(int);
void            pinit(void);
void            procdump(void);
void            scheduler(void) __attribute__((noreturn));
void            setupsegs(struct proc*);
void            sleep(void*, struct spinlock*);
void            userinit(void);
int             wait(void);
void            wakeup(void*);
void            yield(void);

// swtch.S
void            swtch(struct context*, struct context*);

// spinlock.c
void            acquire(struct spinlock*);
void            getcallerpcs(void*, uint*);
int             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            pushcli();
void            popcli();

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
int             fetchint(struct proc*, uint, int*);
int             fetchstr(struct proc*, uint, char**);
void            syscall(void);

// timer.c
void            timer_init(void);

// trap.c
void            idtinit(void);
extern int      ticks;
void            tvinit(void);
extern struct spinlock tickslock;

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x)/sizeof((x)[0]))

