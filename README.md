## CS-372 Project 4 - Juntao Wang
### System Call Trace
* System call trace is disabled by default
* To enable: 
    - open `syscall.c`
    - go to `void syscall(void)`
    - near line 223, find `ShowSysCall` function call
    - change first argument `SHOW_SYS_FALSE` to `SHOW_SYS_TRUE`
    - alternatively, set the first argument to 1
    - compile the kernal
* To disable:
    - change first argument `SHOW_SYS_TRUE` to `SHOW_SYS_FALSE`
    - alternatively, set the first argument to 0
    - compile the kernal

### Date System Call:
* Files modified for creating `date` system call:
    - `user.h`: added `int date(struct rtcdate *);`
    - `usys.S`: added `SYSCALL(date)` 
    - `syscall.h`: added `#define SYS_date 22`
    - `syscall.c`: added `extern int sys_date(void);`, and `[SYS_date] sys_date` in `static int (*syscalls[])(void)`, and system call trace in `ShowSysCall`
    - `sysproc.c`:added the following function
    - `Makefile`: added `_date\` to the end of `UPROGS`
    
        ```
        // print current UTC time
        int sys_date(void)
        {
            struct rtcdate *date;
            if (argptr(0, (void *)&date, sizeof(*date)) < 0)
                return -1;

            cmostime(date);
            return 0;
        }
        ```
* Added `date.c` as a user program for calling `date` system call.
    - type `date` in xv-6 shell to check current UTC time

