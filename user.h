#ifndef XV6_USER_H
#define XV6_USER_H

#include "types.h"
#include "ioctl_request.h"

struct stat;
struct rtcdate;

#define stderr 2

// system calls
int fork(void);
int exit(int status) __attribute__((noreturn));
int wait(int* wstatus);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(const char*, int);
int mknod(char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(char*, char*);
int mkdir(const char*);
int chdir(char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int usleep(unsigned int);
int uptime(void);
int ioctl(int fd, unsigned long request, ...);
int getppid(void);
int getcpu(void);
int getmem(void);
int kmemtest(void);

int mount(const char*, const char*, const char *);
int umount(const char*);
int printmounts(void);
int printdevices(void);
int unshare(int);

// ulib.c
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
int strncmp(const char*, const char*, int);
int printf(int, const char*, ...);
char* gets(char*, int max);
uint strlen(const char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);
int itoa(char *, int);
char* strcat(char* dest, const char* source);

int attach_tty(int tty_fd);
int detach_tty(int tty_fd);
int connect_tty(int tty_fd);
int is_attached_tty(int tty_fd);
int disconnect_tty(int tty_fd);
int is_connected_tty(int tty_fd);


#endif /* XV6_USER_H */

