#ifndef TYPES_H
#define TYPES_H

typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef uint pde_t;

enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

#define MAX_PROC_NAME_LENGTH (16)
struct procInfo {
    int state;
    int pid;
    int cpuPercent;
    char name[MAX_PROC_NAME_LENGTH];
};

#endif // TYPES_H
