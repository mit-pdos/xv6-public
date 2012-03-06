#include "types.h"
#include "kernel.hh"
#include "spinlock.h"
#include "condvar.h"
#include "cpu.hh"
#include "proc.hh"
#include "fs.h"
#include "wq.hh"
#include "ipc.hh"
#include "vm.hh"
#include "file.hh"

static_assert(sizeof(struct ipcctl) < PGSIZE, "struct ipcctl too large");

static void
pread_work(void *a0, void *a1, void *a2,
           void *a3, void *a4)
{
  struct inode *ip = (inode*) a0;
  size_t count = (uptr)a1;
  off_t off = (uptr)a2;
  struct ipcmsg *msg = (struct ipcmsg*) a3;
  void *ubuf = (void*) a4;
  int r;

  ilock(ip, 0);
  r = readi(ip, (char*)ubuf, off, count);
  iunlock(ip);
  
  msg->result = r;
  barrier();
  msg->done = 1;

  iput(ip);
}

static cwork *
pread_allocwork(struct inode *ip, size_t count, off_t off,
                struct ipcmsg *msg, void *ubuf)
{
  cwork *w = new cwork();
  if (w == nullptr)
    return 0;

  w->rip = (void*) pread_work;
  w->arg0 = ip;
  w->arg1 = (void*)count;
  w->arg2 = (void*)off;
  w->arg3 = msg;
  w->arg4 = ubuf;

  return w;
}

long
sys_async(int fd, size_t count, off_t off,
          msgid_t msgid, pageid_t pageid)
{
  struct file *f;
  cwork *w;

  char *kshared = myproc()->vmap->kshared;
  struct ipcctl *ipcctl = (struct ipcctl*)kshared;
  struct ipcmsg *msg;
  void *ubuf;

  if (msgid > IPC_NMSG)
    return -1;
  if (pageid > IPC_NPAGE)
    return -1;
  
  msg = &ipcctl->msg[msgid];
  ubuf = (kshared+PGSIZE+(pageid*PGSIZE));

  if ((f = myproc()->ftable->getfile(fd)) == nullptr)
    return -1;
  if (f->type != file::FD_INODE)
    return -1;

  f->ip->ref++;
  w = pread_allocwork(f->ip, count, off, msg, ubuf);
  if (w == nullptr) {
    iput(f->ip);
    return -1;
  }
  if (wq_push(w) < 0) {
    iput(f->ip);
    delete w;
    return -1;
  }
  msg->off = off;
  msg->submitted = 1;
  return 0;
}

