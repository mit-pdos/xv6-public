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
struct jmpbuf;
void setupsegs(struct proc *);
struct proc * newproc(void);
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
void mp_startthem(void);
int mp_bcpu(void);

// lapic
extern uint32_t *lapicaddr;
void lapic_init(int);
void lapic_startap(uint8_t, int);
void lapic_timerinit(void);
void lapic_timerintr(void);
void lapic_enableintr(void);
void lapic_disableintr(void);
int cpu(void);

// spinlock.c
struct spinlock;
void acquire(struct spinlock * lock);
void release(struct spinlock * lock);
void acquire1(struct spinlock * lock, struct proc *);
void release1(struct spinlock * lock, struct proc *);

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
void fd_reference(struct fd *fd);

// ide.c
void ide_init(void);
void ide_intr(void);
void* ide_start_read(uint32_t secno, void *dst, unsigned nsecs);
int ide_finish_read(void *);
