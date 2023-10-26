#include "param.h"
struct stat;
struct rtcdate;

// system calls
int fork(void);
int exit(void) __attribute__((noreturn));
int wait(void);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(const char*, int);
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
int yield(void);
int shutdown(void);

/// @brief This is a system call to obtain information about existing 
/// processes in the kernel
/// @param count : the maximum number of elements storable in procInfoArray
/// @param procInfoArray : an array of struct procInfo able to store at least 
/// count elements.
/// @return The number of struct procInfo structures stored in procInfoArray
/// by the kernel. This number may be less than count, and if it is, elements
/// at indexes >= count may contain uninitialized memory.
int ps(int count, struct procInfo* procInfoArray);

/// @brief Make system call to obtain user virtual address of start
/// of shared memory page.
/// @param sharedPageStart_p : pointer to the memory that will store
/// the user virtual address of start of shared memory page
/// @return zero upon success and -1 otherwise
int attachSharedMemory(char **sharedPageStart_p);

// ulib.c
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void printf(int, const char*, ...);
int snprintf(char *outbuffer, int n, const char *fmt, ...);
char* gets(char*, int max);
uint strlen(const char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);
