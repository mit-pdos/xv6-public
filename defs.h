// kalloc.c
char *kalloc(int n);
void kfree(char *cp, int len);

// console.c
void cprintf(char *fmt, ...);
void panic(char *s);

// proc.c
struct proc;
void setupsegs(struct proc *p);
struct proc * newproc(struct proc *op);
