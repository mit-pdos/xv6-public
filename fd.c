#include "types.h"
#include "stat.h"
#include "param.h"
#include "x86.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "fd.h"
#include "spinlock.h"
#include "dev.h"

struct spinlock fd_table_lock;
struct devsw devsw[NDEV];

struct fd fds[NFD];

void
fd_init(void)
{
  initlock(&fd_table_lock, "fd_table");
}

/*
 * allocate a file descriptor number for curproc.
 */
int
fd_ualloc(void)
{
  int fd;
  struct proc *p = curproc[cpu()];
  for(fd = 0; fd < NOFILE; fd++)
    if(p->fds[fd] == 0)
      return fd;
  return -1;
}

/*
 * allocate a file descriptor structure
 */
struct fd *
fd_alloc(void)
{
  int i;

  acquire(&fd_table_lock);
  for(i = 0; i < NFD; i++){
    if(fds[i].type == FD_CLOSED){
      fds[i].type = FD_NONE;
      fds[i].ref = 1;
      release(&fd_table_lock);
      return fds + i;
    }
  }
  release(&fd_table_lock);
  return 0;
}

/*
 * addr is a kernel address, pointing into some process's p->mem.
 */
int
fd_write(struct fd *fd, char *addr, int n)
{
  if(fd->writeable == 0)
    return -1;
  if(fd->type == FD_PIPE){
    return pipe_write(fd->pipe, addr, n);
  } else if (fd->type == FD_FILE) {
    ilock(fd->ip);
    int r = writei (fd->ip, addr, fd->off, n);
    if (r > 0) {
      fd->off += r;
    }
    iunlock(fd->ip);
    return r;
  } else {
    panic("fd_write");
    return -1;
  }
}

int
fd_read(struct fd *fd, char *addr, int n)
{
  if(fd->readable == 0)
    return -1;
  if(fd->type == FD_PIPE){
    return pipe_read(fd->pipe, addr, n);
  } else if(fd->type == FD_FILE){
    ilock(fd->ip);
    int cc = readi(fd->ip, addr, fd->off, n);
    if(cc > 0)
      fd->off += cc;
    iunlock(fd->ip);
    return cc;
  } else {
    panic("fd_read");
    return -1;
  }
}

void
fd_close(struct fd *fd)
{
  acquire(&fd_table_lock);

  if(fd->ref < 1 || fd->type == FD_CLOSED)
    panic("fd_close");

  if(--fd->ref == 0){
    struct fd dummy = *fd;

    fd->ref = 0;
    fd->type = FD_CLOSED;
    release(&fd_table_lock);

    if(dummy.type == FD_PIPE){
      pipe_close(dummy.pipe, dummy.writeable);
    } else if(dummy.type == FD_FILE){
      idecref(dummy.ip);
    } else {
      panic("fd_close");
    }
  } else {
    release(&fd_table_lock);
  }
}

int
fd_stat(struct fd *fd, struct stat *st)
{
  if(fd->type == FD_FILE){
    ilock(fd->ip);
    stati(fd->ip, st);
    iunlock(fd->ip);
    return 0;
  } else
    return -1;
}

void
fd_incref(struct fd *fd)
{
  acquire(&fd_table_lock);
  if(fd->ref < 1 || fd->type == FD_CLOSED)
    panic("fd_incref");
  fd->ref++;
  release(&fd_table_lock);
}
