#include "mmu.h"
#include "lib.h"

#define KBASE   0xFFFFFF0000000000ull
#define USERTOP 0x0000800000000000ull

#define KCSEG (2<<3)  /* kernel code segment */
#define KDSEG (3<<3)  /* kernel data segment */

static inline uptr v2p(void *a) { return (uptr) a  - KBASE; }
static inline void *p2v(uptr a) { return (void *) a + KBASE; }

struct trapframe;
struct spinlock;
struct condvar;
struct wqframe;
struct context;
struct vmnode;
struct inode;
struct node;
struct file;
struct stat;
struct proc;
struct vmap;
struct pipe;

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
void            cv_sleep(struct condvar *cv, struct spinlock*);
void            cv_sleepto(struct condvar *cv, struct spinlock*, u64);
void            cv_wakeup(struct condvar *cv);
void            cv_tick(void);

// console.c
void            cprintf(const char*, ...) __attribute__((format(printf, 1, 2)));
void            panic(const char*, ...) 
                  __noret__ __attribute__((format(printf, 1, 2)));
void            kerneltrap(struct trapframe *tf) __noret__;
void            snprintf(char *buf, u32 n, char *fmt, ...);
void            consoleintr(int(*)(void));

#define assert(c)   if (!(c)) { cprintf("%s:%d: ", __FILE__, __LINE__); panic("assertion failure"); }

// crange.c

struct clist_range {
  u64 key;
  u64 size;
  void *value;
  int curlevel;
  int nlevel;
  struct crange *cr;
  struct clist_range** next;  // one next pointer per level
  struct spinlock *lock; // on separate cache line?
} __mpalign__;

struct crange;

struct crange* crange_alloc(int nlevel);
void crange_free(struct crange *cr);
void crange_del(struct crange *cr, u64 k, u64 sz);
void crange_add(struct crange *cr, u64 k, u64 sz, void *v);
struct clist_range* crange_search(struct crange *cr, u64 k, u64 sz);
int crange_foreach(struct crange *crk, int (*f)(struct clist_range *r, void *st), void *st);
void crange_print(struct crange *cr, int);

// e1000.c
extern int e1000irq;
extern int e1000init;
void            e1000intr(void);
int             e1000tx(void *buf, u32 len);
void            e1000hwaddr(u8 *hwaddr);

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
int             namecmp(const char*, const char*);
struct inode*   dirlookup(struct inode*, char*);
struct inode*   ialloc(u32, short);
struct inode*   namei(char*);
void            iput(struct inode*);
struct inode*   iget(u32 dev, u32 inum);
void            ilock(struct inode*, int writer);
void            iunlockput(struct inode*);
void            iupdate(struct inode*);
void            iunlock(struct inode*);
int             readi(struct inode*, char*, u32, u32);
void            stati(struct inode*, struct stat*);
int             writei(struct inode*, char*, u32, u32);
struct inode*   idup(struct inode*);
struct inode*   nameiparent(char*, char*);
int             dirlink(struct inode*, char*, u32);
void            dir_init(struct inode *dp);
void	        dir_flush(struct inode *dp);

// gc.c
void            initgc(void);
void            initprocgc(struct proc *);
void            gc_begin_epoch();
void            gc_end_epoch();
void            gc_delayed(void*, void (*dofree)(void*));
void            gc_delayed2(int, u64, void (*dofree)(int, u64));
void            gc_start(void);

// hwvm.c
void            freevm(pml4e_t*);
pml4e_t*        setupkvm(void);
pme_t *         walkpgdir(pml4e_t*, const void*, int);

// hz.c
void            microdelay(u64);
u64             nsectime(void);

// ide.c
void            ideinit(void);
void            ideintr(void);
void            iderw(struct buf*);

// ioapic.c
void            ioapicenable(int irq, int cpu);

// kalloc.c
typedef enum {
  slab_stack,
  slab_perf,
} slab_t;
char*           kalloc(void);
void            kfree(void*);
void*           ksalloc(slab_t);
void            ksfree(slab_t, void*);
void*           kmalloc(u64);
void            kmfree(void*);
int             kmalign(void **p, int align, u64 size);
void            kmalignfree(void *);


// kbd.c
void            kbdintr(void);

// lapic.c
int             cpunum(void);
void            lapicstartap(u8, u32 addr);
void            lapiceoi(void);
void            lapic_tlbflush(u32);
void            lapic_sampconf(u32);
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

// ns.c
enum {
  nskey_int = 1,
  nskey_ii,
  nskey_str,
  nskey_dirname,
  nskey_iis
};

struct nskey {
  int type;
  union {
    u64 i;
    struct {
      u64 a;
      u64 b;
    } ii;
    char *s;
    char *dirname;
    struct {
      u64 a;
      u64 b;
      char *s;
    } iis;
  } u;
};

#define KI(v)	    (struct nskey){.type=nskey_int,.u.i=v}
#define KII(x,y)    (struct nskey){.type=nskey_ii,.u.ii.a=x,.u.ii.b=y}
#define KS(v)	    (struct nskey){.type=nskey_str,.u.s=v}
#define KD(v)	    (struct nskey){.type=nskey_dirname,.u.dirname=v}
#define KIIS(x,y,z) (struct nskey){.type=nskey_iis,.u.iis.a=x, \
						   .u.iis.b=y, \
						   .u.iis.s=z}

void            nsinit(void);
struct ns*      nsalloc(int allowdup);
void		nsfree(struct ns*);
u64             ns_allockey(struct ns*);
int             ns_insert(struct ns*, struct nskey key, void*);
void*           ns_lookup(struct ns*, struct nskey key);
void*           ns_remove(struct ns *ns, struct nskey key, void *val); // removed val
void*           ns_enumerate(struct ns *ns, void *(*f)(void *, void *, void *), void *arg);
void*           ns_enumerate_key(struct ns *ns, struct nskey key, void *(*f)(void *, void *), void *arg);

// picirq.c
void            picenable(int);
void            piceoi(void);

// pipe.c
int             pipealloc(struct file**, struct file**);
void            pipeclose(struct pipe*, int);
int             piperead(struct pipe*, char*, int);
int             pipewrite(struct pipe*, char*, int);

// proc.c
void            addrun(struct proc *);
struct proc*    copyproc(struct proc*);
void            exit(void);
int             fork(int);
int             growproc(int);
int             kill(int);
void            pinit(void);
void            procdumpall(void);
void            scheduler(void) __noret__;
void            sched(void);
void            userinit(void);
int             wait(void);
void            yield(void);
struct proc*    threadalloc(void (*fn)(void*), void *arg);

// prof.c
extern int profenable;
void            profreset(void);
void            profdump(void);


// rnd.c
u64             rnd();

// sampler.c
void            sampstart(void);
int             sampintr(struct trapframe*);
void            sampdump(void);
void            sampconf(void);

// spinlock.c
void            acquire(struct spinlock*);
int             tryacquire(struct spinlock*);
int             holding(struct spinlock*);
void            initlock(struct spinlock*, const char*);
void            release(struct spinlock*);

// syscall.c
int             argint64(int, u64*);
int             argint32(int n, int *ip);
int             argptr(int, char**, int);
int             argstr(int, char**);
int             fetchint64(uptr, u64*);
int             fetchstr(uptr, char**);
int             umemcpy(void*, void*, u64);
int             kmemcpy(void*, void*, u64);
void            syscall(void);

// string.c
int             memcmp(const void*, const void*, u32);
void*           memmove(void*, const void*, u32);
void*           memset(void*, int, u32);
char*           safestrcpy(char*, const char*, int);
int             strlen(const char*);
int             strncmp(const char*, const char*, u32);
char*           strncpy(char*, const char*, int);
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
struct vmap *   vmap_alloc(void);
struct vmnode*  vmn_alloc(u64, u32);
struct vmnode*  vmn_allocpg(u64);
int             vmap_insert(struct vmap*, struct vmnode *, uptr);
struct vma *    vmap_lookup(struct vmap*, uptr, uptr);
int             copyout(struct vmap *, uptr, void*, u64);
void            vmn_free(struct vmnode *);
void            switchuvm(struct proc*);
void            switchkvm(void);
int             pagefault(struct vmap *, uptr, u32);
void            vmap_decref(struct vmap *);
int             vmn_load(struct vmnode *, struct inode*, u64, u64);
int             vmap_remove(struct vmap *, uptr, u64);
void            updatepages(pml4e_t*, void*, void*, int);
struct vmap *   vmap_copy(struct vmap *, int);

// wq.c
#if WQENABLE
void            wq_push(void *rip, u64 arg0, u64 arg1);
void            wq_start(void);
void            wq_end(void);
void            wq_dump(void);
int             wq_trywork(void);
void            initwqframe(struct wqframe *wq);
#else
#define wq_push(rip, arg0, arg1) do { \
  void (*fn)(uptr, uptr) = rip; \
  fn(arg0, arg1); \
} while(0)
#define wq_start() do { } while(0)
#define wq_end() do { } while(0)
#define wq_dump() do { } while(0)
#define wq_trywork() 0
#define initwqframe(x) do { } while (0)
#endif
