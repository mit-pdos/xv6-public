int fork(void);
int exit(void) __attribute__((noreturn));
int wait(void);
int cons_putc(int);
int pipe(int*);
int write(int, void*, int);
int read(int, void*, int);
int close(int);
int block(void);
int kill(int);
int panic(char*);
int cons_puts(char*);

int puts(char*);
int puts1(char*);
char* strcpy(char*, char*);
