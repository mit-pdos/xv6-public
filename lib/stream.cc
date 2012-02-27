#include "types.h"
#include "stat.h"
#include "user.h"
#include "stream.h"
#include "lib.h"
#include "amd64.h"
#include "ipc.hh"

static const size_t pstride = 4096*4;

static ssize_t
fasync(FILE *fp, size_t count, off_t off)
{
  struct ipcmsg *msg;
  msgid_t msgid;
  pageid_t pageid;

  msgid = ipc_msg_alloc();
  if (msgid == nullptr_MSGID) {
    fprintf(2, "fasync: ipc_msg_alloc failed\n");
    return -1;
  }

  pageid = ipc_page_alloc();
  if (pageid == nullptr_PAGEID) {
    fprintf(2, "fasync: ipc_alloc_page failed\n");
    return -1;
  }

  msg = &ipcctl->msg[msgid];
  msg->done = 0;
  msg->pageid = pageid;

  if (async(fp->fd, count, off, msgid, pageid) != 0) {
    fprintf(2, "fasync: async failed\n");
    return -1;
  }

  return count;
}

static void
fprefill(FILE *fp)
{
  size_t target;

  if (!fp->pfill)
    return;
  
  target = MIN(fp->off + pstride, fp->stat.size);
  while (target - fp->poff >= IPC_PGSIZE)
  {
    size_t count;
    int r;

    count = MIN(target - fp->poff, IPC_PGSIZE);
    r = fasync(fp, count, fp->poff);
    if (r < 0)
      return;
    fp->poff += r;
  }
}

FILE*
fdopen(int fd, const char *mode)
{
  FILE *fp;

  if (mode[0] != 'r')
    return 0;
  fp = (FILE*)malloc(sizeof(*fp));
  if (fp == 0)
    return 0;
  if (fstat(fd, &fp->stat))
    return 0;
  fp->fd = fd;
  fp->off = 0;
  fp->poff = 0;
  fp->pfill = mode[1] == 'p';
  fprefill(fp);
  return fp;
}

int
fclose(FILE *fp)
{
  int r;

  r = close(fp->fd);
  free(fp);
  return r;
  // XXX(sbw) free ipcmsgs
}

static ssize_t
fpostfill(void *ptr, size_t count, FILE*fp)
{
  struct ipcmsg *msg;
  msgid_t msgid;

  if (!fp->pfill)
    return -2;

again:
  msgid = ipcctl->msgtail % IPC_NMSG;
  msg = getmsg(msgid);

  if (!msg->submitted)
    return -2;
  while (msg->done == 0)
    nop_pause(); // XXX(sbw) yield somewhere?
  if (msg->result == -1)
    return -1;

  if (msg->off > fp->off) {
    return -2;
  } else if ((msg->off + msg->result) < fp->off) {
    msg->submitted = 0;
    ipc_page_free(msg->pageid);
    ipc_msg_free(msgid);
    goto again;
  }

  char *buf = getpage(msg->pageid);
  off_t boff = fp->off - msg->off;
  size_t bcount = MIN(count, msg->result-boff);
  memmove(ptr, buf+boff, bcount);
  
  msg->submitted = 0;
  ipc_page_free(msg->pageid);
  ipc_msg_free(msgid);
  return bcount;
}

size_t
fread(void *ptr, size_t size, size_t nmemb, FILE *fp)
{
  ssize_t r;

  r = fpostfill(ptr, size*nmemb, fp);
  if (r == -2)
    r = pread(fp->fd, ptr, size*nmemb, fp->off);

  if (r < 0) {
    fp->err = 1;
    return 0;
  } else if (r == 0) {
    fp->eof = 1;
    return 0;
  }
  fp->off += r;
  fprefill(fp);
  return r;
}

int
feof(FILE *fp)
{
  return fp->eof;
}

int
ferror(FILE *fp)
{
  return fp->err;
}
