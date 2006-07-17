#include "types.h"
#include "param.h"
#include "x86.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "fd.h"
#include "spinlock.h"

struct spinlock fd_table_lock;

struct fd fds[NFD];

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
    if(fd->type == FD_PIPE){
      pipe_close(fd->pipe, fd->writeable);
    } else {
      panic("fd_close");
    }
    fd->ref = 0;
    fd->type = FD_CLOSED;
  }
  
  release(&fd_table_lock);
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
