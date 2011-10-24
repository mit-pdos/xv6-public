#define KBASE 0xFFFFFFFF80000000ull
#define PBASE 0xFFFFFF0000000000ull

#define KCSEG (2<<3)  /* kernel code segment */
#define KDSEG (3<<3)  /* kernel data segment */

#define NULL ((void *)0)

static inline uptr v2p(void *a) { return (uptr) a  - KBASE; }
static inline void *p2v(uptr a) { return (void *) a + KBASE; }

struct spinlock;
struct condvar;
struct proc;

// cga.c
void            cgaputc(char c);

// condvar.c
void            initcondvar(struct condvar *, char *);
void            cv_sleep(struct condvar *cv, struct spinlock*);
void            cv_wakeup(struct condvar *cv);

// console.c
void            cprintf(const char*, ...);
void            panic(const char*) __attribute__((noreturn));

// kalloc.c
char*           kalloc(void);
void            kfree(void *);
void*           kmalloc(u64);
void            kmfree(void*);

// lapic.c
int             cpunum(void);

// mp.c
extern int      ncpu;

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
int             ns_allockey(struct ns*);
int             ns_insert(struct ns*, struct nskey key, void*);
void*           ns_lookup(struct ns*, struct nskey key);
void*           ns_remove(struct ns *ns, struct nskey key, void *val); // removed val
void*           ns_enumerate(struct ns *ns, void *(*f)(void *, void *, void *), void *arg);
void*           ns_enumerate_key(struct ns *ns, struct nskey key, void *(*f)(void *, void *), void *arg);

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

// spinlock.c
void            acquire(struct spinlock*);
void            getcallerpcs(void*, uptr*);
int             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            pushcli(void);
void            popcli(void);

// string.c
int             memcmp(const void*, const void*, u32);
void*           memmove(void*, const void*, u32);
void*           memset(void*, int, u32);
char*           safestrcpy(char*, const char*, int);
int             strlen(const char*);
int             strncmp(const char*, const char*, u32);
char*           strncpy(char*, const char*, int);
int             strcmp(const char *p, const char *q);

// uart.c
void            uartputc(char c);
