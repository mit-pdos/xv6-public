struct buf;
struct context;
struct file;
struct inode;
struct mount;
struct mount_list;
struct mount_ns;
struct nsproxy;
struct pipe;
struct proc;
struct rtcdate;
struct spinlock;
struct sleeplock;
struct stat;
struct superblock;
struct cgroup;

// bio.c
void            binit(void);
struct buf*     bread(uint, uint);
void            brelse(struct buf*);
void            bwrite(struct buf*);
void            invalidateblocks(uint);

// console.c
void            consoleclear(void);
void            consoleinit(void);
void		    ttyinit(void);
void            cprintf(char*, ...);
void            consoleintr(int(*)(void));
void            panic(char*) __attribute__((noreturn));
void            tty_disconnect(struct inode *ip);
void            tty_connect(struct inode *ip);
void            tty_attach(struct inode *ip);
void            tty_detach(struct inode *ip);
int             tty_gets(struct inode *ip, int command);

// device.c
int             getorcreatedevice(struct inode*);
void            deviceput(uint);
void            deviceget(uint);
void            printdevices(void);
struct inode*   getinodefordevice(uint);
struct superblock* getsuperblock(uint);
void            devinit(void);
int             doesbackdevice(struct inode*);

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
void            readsb(int dev, struct superblock *sb);
int             dirlink(struct inode*, char*, uint);
struct inode*   dirlookup(struct inode*, char*, uint*);
struct inode*   ialloc(uint, short);
struct inode*   idup(struct inode*);
void            iinit(uint dev);
void            ilock(struct inode*);
void            iput(struct inode*);
void            iunlock(struct inode*);
void            iunlockput(struct inode*);
void            iupdate(struct inode*);
int             namecmp(const char*, const char*);
struct inode*   namei(char*);
struct inode*   nameimount(char*, struct mount**);
struct inode*   nameiparent(char*, char*);
struct inode*   nameiparentmount(char*, char*, struct mount**);
int             readi(struct inode*, char*, uint, uint);
void            stati(struct inode*, struct stat*);
int             writei(struct inode*, char*, uint, uint);
void            readsb(int, struct superblock *);
void            fsinit(uint);
struct inode*   initprocessroot(struct mount**);

// mount.c
void            mntinit(void);
void            printmounts(void);
int             mount(struct inode*, struct inode*, struct mount*);
int             umount(struct mount*);
struct mount*   getrootmount(void);
struct mount*   mntdup(struct mount*);
void            mntput(struct mount*);
struct mount*   mntlookup(struct inode*, struct mount*);
void            umountall(struct mount_list*);
struct mount_list* copyactivemounts(void);
struct mount*   getroot(struct mount_list*);
struct mount*   getinitialrootmount(void);

// ide.c
void            ideinit(void);
void            ideintr(void);
void            iderw(struct buf*);

// ioapic.c
void            ioapicenable(int irq, int cpu);
extern uchar    ioapicid;
void            ioapicinit(void);

// kalloc.c
char*           kalloc(void);
void            kfree(char*);
void            kinit1(void*, void*);
void            kinit2(void*, void*);
int             kmemtest(void);

// kbd.c
void            kbdintr(void);

// lapic.c
void            cmostime(struct rtcdate *r);
int             lapicid(void);
extern volatile uint*    lapic;
void            lapiceoi(void);
void            lapicinit(void);
void            lapicstartap(uchar, uint);
void            microdelay(int);

// log.c
void            initlog(int dev);
void            log_write(struct buf*);
void            begin_op();
void            end_op();

// mount_ns.c
void            mount_nsinit(void);
void            mount_nsput(struct mount_ns*);
struct mount_ns* mount_nsdup(struct mount_ns*);
struct mount_ns* newmount_ns(void);
struct mount_ns* copymount_ns(void);

// mp.c
extern int      ismp;
void            mpinit(void);

// namespace.c
void            namespaceinit(void);
struct nsproxy* emptynsproxy(void);
struct nsproxy* namespacedup(struct nsproxy*);
void            namespaceput(struct nsproxy*);
int             unshare(int nstype);

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
int             cpuid(void);
void            exit(int);
int             fork(void);
int             growproc(int);
int             kill(int);
struct cpu*     mycpu(void);
struct proc*    myproc();
void            pinit(void);
void            procdump(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
void            setproc(struct proc*);
void            sleep(void*, struct spinlock*);
void            userinit(void);
int             wait(int*);
void            wakeup(void*);
void            yield(void);
int             cgroup_move_proc(struct cgroup * cgroup, int pid);

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

// sleeplock.c
void            acquiresleep(struct sleeplock*);
void            releasesleep(struct sleeplock*);
int             holdingsleep(struct sleeplock*);
void            initsleeplock(struct sleeplock*, char*);

// string.c
int             memcmp(const void*, const void*, uint);
void*           memmove(void*, const void*, uint);
void*           memset(void*, int, uint);
char*           safestrcpy(char*, const char*, int);
int             strlen(const char*);
int             strncmp(const char*, const char*, uint);
int             strcmp(const char * p, const char * q);
char*           strncpy(char*, const char*, int);

// syscall.c
int             argint(int, int*);
int             argptr(int, char**, int);
int             argstr(int, char**);
int             fetchint(uint, int*);
int             fetchstr(uint, char**);
void            syscall(void);
int             getppid(void);

// timer.c
void            timerinit(void);

// trap.c
void            idtinit(void);
extern uint     ticks;
void            tvinit(void);
extern struct spinlock tickslock;

// uart.c
void            uartinit(void);
void            uartintr(void);
void            uartputc(int);

// vm.c
void            seginit(void);
void            kvmalloc(void);
pde_t*          setupkvm(void);
char*           uva2ka(pde_t*, char*);
int             allocuvm(pde_t*, uint, uint);
int             deallocuvm(pde_t*, uint, uint);
void            freevm(pde_t*);
void            inituvm(pde_t*, char*, uint);
int             loaduvm(pde_t*, char*, struct inode*, uint, uint);
pde_t*          copyuvm(pde_t*, uint);
void            switchuvm(struct proc*);
void            switchkvm(void);
int             copyout(pde_t*, uint, void*, uint);
void            clearpteu(pde_t *pgdir, char *uva);

// cgroup.c
void            cginit(void);

// klib.c
int             atoi(char * str);
int             itoa(char * buf, int n);
int             utoa(char * buf, unsigned int n);
int             intlen(int n);


// number of elements in fixed-size array
#define NELEM(x) (sizeof(x)/sizeof((x)[0]))
