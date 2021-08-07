#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "steady_clock.h"
#include "ioctl_request.h"
#include "fcntl.h"
#include "file.h"


// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
static int
argfd(int n, int *pfd, struct file **pf)
{
  int fd;
  struct file *f;

  if(argint(n, &fd) < 0)
    return -1;
  if(fd < 0 || fd >= NOFILE || (f=myproc()->ofile[fd]) == 0)
    return -1;
  if(pfd)
    *pfd = fd;
  if(pf)
    *pf = f;
  return 0;
}

int
sys_fork(void)
{
  return fork();
}

int
sys_exit()
{
  int status;

  if (argint(0, &status) < 0)
    return -1;
  exit(status);
  return 0;  // not reached
}

int
sys_wait(void)
{
  int* wstatus;

  if (argptr(0, (void*)&wstatus, sizeof(*wstatus)) < 0)
    return -1;
  return wait(wstatus);
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->ns_pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

int
sys_usleep(void)
{
  int n;

  if(argint(0, &n) < 0)
    return -1;
  unsigned int start = steady_clock_now();
  acquire(&tickslock);
  while(steady_clock_now() - start < (unsigned int)n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

int
sys_ioctl(void)
{
  int fd = -1;
  int request = -1;
  int command;

  int ret;
  struct file *f;
  struct inode* ip;

  if(argfd(0, &fd, &f) < 0 || argint(1, &request) < 0 || argint(2, &command) < 0)
    return -1;

  if(!(command  & DEV_CONNECT) &&
     !(command & DEV_DISCONNECT) &&
     !(command  & DEV_DETACH) &&
     !(command & DEV_ATTACH)){
    return -1;
  }

  ip = f->ip;

  if(ip->minor == CONSOLE_MINOR){
    return -1;
  }

  ilock(ip);

  if( ip->type != T_DEV ){
      iunlockput(ip);
      return -1;
  }

  if(ip->major >= NDEV){
     iunlockput(ip);
     return -1;
  }

  if(ip->minor >= MAX_TTY){
     iunlockput(ip);
     return -1;
  }

  int result;
  switch (request) {
  case IOCTL_GET_PROCESS_CPU_PERCENT:
    proc_lock();
    result = myproc()->cpu_percent;
    proc_unlock();
    return result;
  case IOCTL_GET_PROCESS_CPU_TIME:
    proc_lock();
    result = myproc()->cpu_time;
    proc_unlock();
    return result;

  case TTYSETS:
    if((command & DEV_DISCONNECT)){
      tty_disconnect(ip);
     }

     if((command & DEV_CONNECT)){
       tty_connect(ip);
     }

     if((command & DEV_ATTACH)){
       tty_attach(ip);
     }

     if((command & DEV_DETACH)){
        tty_detach(ip);
       }
    break;
  case TTYGETS:
    ret = tty_gets(ip, command);
    iunlock(ip);
    return ret;
  default:
    iunlock(ip);
    return -1;
  }

 iunlock(ip);
 return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_getppid(void){
    return myproc()->parent->ns_pid;
}

int
sys_getcpu(void) {
    cli();
    int id = cpuid();
    sti();
    return id;
}

// This is our solution for what can be found at the /proc
// virtual filesystem in linux.
int
sys_getmem(void) {
  return myproc()->sz;
}

int
sys_kmemtest(void) {
  return kmemtest();
}
