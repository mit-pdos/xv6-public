// kalloc.c
char *kalloc(int n);
void kfree(char *cp, int len);
void kinit(void);

// console.c
void cprintf(char *fmt, ...);
void panic(char *s);
void cons_putc(int);

// proc.c
struct proc;
void setupsegs(struct proc *);
struct proc * newproc(void);
void swtch(void);
void sleep(void *);
void wakeup(void *);

// trap.c
void tvinit(void);
void idtinit(void);

// string.c
void * memcpy(void *dst, void *src, unsigned n);
void * memset(void *dst, int c, unsigned n);
int memcmp(const void *v1, const void *v2, unsigned n);
void *memmove(void *dst, const void *src, unsigned n);
int strncmp(const char *p, const char *q, unsigned n);

// syscall.c
void syscall(void);

// picirq.c
extern uint16_t irq_mask_8259A;
void irq_setmask_8259A(uint16_t mask);
void pic_init(void);

// mp.c
void mp_init(void);
int cpu(void);
int mp_isbcpu(void);
void lapic_init(int);
void lapic_timerinit(void);
void lapic_timerintr(void);
void lapic_enableintr(void);
void lapic_disableintr(void);

// spinlock.c
extern uint32_t kernel_lock;
void acquire_spinlock(uint32_t* lock);
void release_spinlock(uint32_t* lock);
void release_grant_spinlock(uint32_t* lock, int cpu);

// main.c
void load_icode(struct proc *p, uint8_t *binary, unsigned size);

// pipe.c
struct pipe;
struct fd;
int pipe_alloc(struct fd **fd1, struct fd **fd2);
void pipe_close(struct pipe *p, int writeable);
int pipe_write(struct pipe *p, char *addr, int n);
int pipe_read(struct pipe *p, char *addr, int n);

// fd.c
int fd_ualloc();
struct fd * fd_alloc();
void fd_close(struct fd *);
int fd_read(struct fd *fd, char *addr, int n);
int fd_write(struct fd *fd, char *addr, int n);

// ide.c
extern int disk_channel;
void ide_init(void);
void ide_intr(void);
int ide_start_read(uint32_t secno, void *dst, unsigned nsecs);
int ide_read(uint32_t secno, void *dst, unsigned nsecs);
