#define PGSIZE 4096
#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "lib.h"
#include "amd64.h"
#include "ipc.hh"

#define FSIZE (64 << 10)
#define BSIZE 4096
#define PSIZE (4*BSIZE)

static int use_async;

static char buf[BSIZE];

struct {
  u64 acount;
  u64 atot;

  u64 pcount;
  u64 ptot;
} stats;

static void
kernlet_pread(int fd, size_t count, off_t off)
{
  struct ipcmsg *msg;
  msgid_t msgid;
  pageid_t pageid;
  u64 t0, t1;

  if (!use_async)
    return;

  t0 = rdtsc();
  msgid = ipc_msg_alloc();
  if (msgid == NULL_MSGID) {
    fprintf(2, "kernlet_pread: ipc_alloc_msg failed");
    return;
  }    

  if (count > PGSIZE)
    die("kernlet_pread: count oops");
  pageid = ipc_page_alloc();
  if (pageid == NULL_PAGEID) {
    fprintf(2, "kernlet_pread: ipc_alloc_page failed");
    return;
  }

  msg = &ipcctl->msg[msgid];
  msg->done = 0;
  msg->pageid = pageid;

  if (async(fd, count, off, msgid, pageid) != 0)
    die("kernlet");

  t1 = rdtsc();
  stats.acount++;
  stats.atot += t1 - t0;
}

static ssize_t
xpread(int fd, void *buf, size_t count, off_t off)
{
  struct ipcmsg *msg;
  u64 t0, t1;
  int msgid;

  t0 = rdtsc();
  msgid = ipcctl->msgtail % IPC_NMSG;
  msg = &ipcctl->msg[msgid];

  if (msg->submitted) {
    while (msg->done == 0)
      nop_pause();
    if (msg->result == -1)
      die("xpread: result oops");
    if (msg->off != off)
      die("xpread: off oops %lu", off);

    char *kbuf = (char*) (KSHARED+PGSIZE+(msg->pageid*PGSIZE));
    off_t kbufoff = off - msg->off;
    size_t kbufcount = MIN(count, msg->result - kbufoff);

    memmove(buf, kbuf+kbufoff, kbufcount);
    ipc_msg_free(msgid);
    ipc_page_free(msg->pageid);
    t1 = rdtsc();
    stats.pcount++;
    stats.ptot += t1 - t0;
    return kbufcount;
  }

  ssize_t r = pread(fd, buf, count, off);
  t1 = rdtsc();
  stats.pcount++;
  stats.ptot += t1 - t0;
  return r;
}

int
main(int ac, char **av)
{
  u64 t0, t1;
  int fd;
  int i;

  memset(ipcctl, 0, sizeof(*ipcctl));

  if (ac > 1)
    use_async = av[1][0] == 'a';
  
  fd = open("preadtest.x", O_CREATE|O_RDWR);
  if (fd < 0)
    die("open failed");
  
  for (i = 0; i < FSIZE/BSIZE; i++)
    if (write(fd, buf, BSIZE) != BSIZE)
      die("write failed");

  t0 = rdtsc();
  kernlet_pread(fd, BSIZE, 0*BSIZE);
  kernlet_pread(fd, BSIZE, 1*BSIZE);
  kernlet_pread(fd, BSIZE, 2*BSIZE);
  kernlet_pread(fd, BSIZE, 3*BSIZE);
  kernlet_pread(fd, BSIZE, 4*BSIZE);
  for (i = 0; i < FSIZE; i+=BSIZE) {
    if (xpread(fd, buf, BSIZE, i) != BSIZE)
      die("pread failed");
    if (i+BSIZE+3*BSIZE < FSIZE)
      kernlet_pread(fd, BSIZE, i+BSIZE+4*BSIZE);
  }
  t1 = rdtsc();

  printf("use_async %u\n", use_async);  
  printf("pread: count %lu total %lu ave %lu\n",
         stats.pcount, stats.ptot, stats.ptot / stats.pcount);
  if (use_async)
    printf("async: count %lu total %lu ave %lu\n",
           stats.acount, stats.atot, stats.atot / stats.acount);
  printf("total %lu\n", t1 - t0);  

  exit();
}
