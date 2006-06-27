#include "types.h"
#include "param.h"
#include "x86.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "fd.h"

struct fd fds[NFD];

/*
 * allocate a file descriptor number for curproc.
 */
int
fd_ualloc()
{
  int fd;
  struct proc *p = curproc[cpu()];
  for(fd = 0; fd < NOFILE; fd++)
    if(p->fds[fd] == 0)
      return fd;
  return -1;
}

struct fd *
fd_alloc()
{
  int i;

  for(i = 0; i < NFD; i++){
    if(fds[i].type == FD_CLOSED){
      fds[i].type = FD_NONE;
      fds[i].count = 1;
      return fds + i;
    }
  }
  return 0;
}

void
fd_close(struct fd *fd)
{
  if(fd->type == FD_CLOSED || fd->count <= 0)
    panic("fd_close");
  fd->count -= 1;
  if(fd->count == 0){
    if(fd->type == FD_PIPE)
      pipe_close(fd->pipe, fd->writeable);
    fd->type = FD_CLOSED;
  }
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
