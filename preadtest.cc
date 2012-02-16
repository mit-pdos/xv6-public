#define PGSIZE 4096
#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "lib.h"
#include "amd64.h"
#include "ipc.hh"

// XXX(sbw) add a memlayout.h?
#define KSHARED 0xFFFFF00000000000ull

#define FSIZE (64 << 10)
#define BSIZE 4096
#define PSIZE (4*BSIZE)

static int usekernlet;

static char buf[BSIZE];

struct ipcctl *ipcctl = (struct ipcctl*)KSHARED;

static msgid_t
ipc_msg_alloc(void)
{
  if (ipcctl->msghead - ipcctl->msgtail == IPC_NMSG)
    return NULL_MSGID;

  msgid_t i = ipcctl->msghead % IPC_NMSG;
  ipcctl->msghead++;
  return i;
}

static void
ipc_msg_free(int msgid)
{
  msgid_t i;

  i = ipcctl->msgtail % IPC_NMSG;
  if (i != msgid)
    die("ipc_free_msg: oops");
  ipcctl->msgtail++;
}

static pageid_t
ipc_page_alloc(void)
{
  if (ipcctl->pagehead - ipcctl->pagetail == IPC_NPAGE)
    return NULL_PAGEID;

  pageid_t i = ipcctl->pagehead % IPC_NPAGE;
  ipcctl->pagehead++;
  return i;
}

static void
ipc_page_free(pageid_t pageid)
{
  pageid_t i;

  i = ipcctl->pagetail % IPC_NPAGE;
  if (i != pageid)
    die("ipc_free_page: oops");
  ipcctl->pagetail++;
}

static void
kernlet_pread(int fd, size_t count, off_t off)
{
  struct ipcmsg *msg;
  msgid_t msgid;
  pageid_t pageid;

  msgid = ipc_msg_alloc();
  if (msgid == NULL_MSGID) {
    printf(2, "kernlet_pread: ipc_alloc_msg failed");
    return;
  }    

  if (count > PGSIZE)
    die("kernlet_pread: count oops");
  pageid = ipc_page_alloc();
  if (pageid == NULL_PAGEID) {
    printf(2, "kernlet_pread: ipc_alloc_page failed");
    return;
  }

  msg = &ipcctl->msg[msgid];
  msg->done = 0;
  msg->pageid = pageid;

  if (kernlet(fd, count, off, msgid, pageid) != 0)
    die("kernlet");
}

static ssize_t
xpread(int fd, void *buf, size_t count, off_t off)
{
  struct ipcmsg *msg;
  int msgid;

  msgid = ipcctl->msgtail % IPC_NMSG;
  msg = &ipcctl->msg[msgid];

  if (msg->submitted) {
    while (msg->done == 0)
      nop_pause();
    if (msg->result == -1)
      die("xpread: result oops");
    if (msg->off != off)
      die("xpread: off oops");

    char *kbuf = (char*) (KSHARED+PGSIZE+(msg->pageid*PGSIZE));
    off_t kbufoff = off - msg->off;
    size_t kbufcount = MIN(count, msg->result - kbufoff);

    memmove(buf, kbuf+kbufoff, kbufcount);
    ipc_msg_free(msgid);
    ipc_page_free(msg->pageid);
    return kbufcount;
  }

  return pread(fd, buf, count, off);
}

int
main(int ac, char **av)
{
  u64 t0, t1;
  int fd;
  int i;

  memset(ipcctl, 0, sizeof(*ipcctl));

  if (ac > 1)
    usekernlet = av[1][0] == 'k';
  
  fd = open("preadtest.x", O_CREATE|O_RDWR);
  if (fd < 0)
    die("open failed");
  
  for (i = 0; i < FSIZE/BSIZE; i++)
    if (write(fd, buf, BSIZE) != BSIZE)
      die("write failed");

  t0 = rdtsc();
  kernlet_pread(fd, BSIZE, 0);
  for (i = 0; i < FSIZE; i+=BSIZE) {
    if (xpread(fd, buf, BSIZE, i) != BSIZE)
      die("pread failed");
    kernlet_pread(fd, BSIZE, i+BSIZE);
  }
  t1 = rdtsc();

  printf(1, "usekernlet %u\n", usekernlet);  
  printf(1, "cycles %lu\n", t1 - t0);
  
  exit();
}
