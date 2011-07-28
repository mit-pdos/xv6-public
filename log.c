#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "spinlock.h"
#include "fs.h"
#include "buf.h"

// Dirt simple "logging" supporting only one transaction.  All file system calls
// that potentially write a block should be wrapped in begin_trans and commit_trans,
// so that there is never more than one transaction. This serializes all file system 
// operations that potentially write, but simplifies recovery (only the last
// one transaction to recover) and concurrency (don't have to worry about reading a modified
// block from a transaction that hasn't committed yet).

// The header of the log.  If head == 0, there are no log entries.  All entries till head
// are committed. sector[] records the home sector for each block in the log 
// (i.e., physical logging).
struct logheader {
  int head;   
  int sector[LOGSIZE];
};

struct {
  struct spinlock lock;
  int start;
  int size;
  int intrans;
  int dev;
  struct logheader lh;
} log;

static void recover_from_log(void);

void
initlog(void)
{
  if (sizeof(struct logheader) >= BSIZE)
    panic("initlog: too big logheader");

  struct superblock sb;
  initlock(&log.lock, "log");
  readsb(ROOTDEV, &sb);
  log.start = sb.size - sb.nlog;
  log.size = sb.nlog;
  log.dev = ROOTDEV;
  recover_from_log();
}

// Copy committed blocks from log to their home location
static void 
install_trans(void)
{
  int tail;

  if (log.lh.head > 0)
    cprintf("install_trans %d\n", log.lh.head);
  for (tail = 0; tail < log.lh.head; tail++) {
    cprintf("put entry %d to disk block %d\n", tail, log.lh.sector[tail]);
    struct buf *lbuf = bread(log.dev, log.start+tail+1);   // read i'th block from log
    struct buf *dbuf = bread(log.dev, log.lh.sector[tail]);  // read dst block
    memmove(dbuf->data, lbuf->data, BSIZE);
    bwrite(dbuf);
    brelse(lbuf);
    brelse(dbuf);
  }
}

// Read the log header from disk into the in-memory log header
static void
read_head(void)
{
  struct buf *buf = bread(log.dev, log.start);
  struct logheader *lh = (struct logheader *) (buf->data);
  int i;
  log.lh.head = lh->head;
  for (i = 0; i < log.lh.head; i++) {
    log.lh.sector[i] = lh->sector[i];
  }
  brelse(buf);
  if (log.lh.head > 0)
    cprintf("read_head: %d\n", log.lh.head);
}

// Write the in-memory log header to disk, committing log entries till head
static void
write_head(void)
{
  if (log.lh.head > 0)
    cprintf("write_head: %d\n", log.lh.head);

  struct buf *buf = bread(log.dev, log.start);
  struct logheader *hb = (struct logheader *) (buf->data);
  int i;
  hb->head = log.lh.head;
  for (i = 0; i < log.lh.head; i++) {
    hb->sector[i] = log.lh.sector[i];
  }
  bwrite(buf);
  brelse(buf);
}

static void
recover_from_log(void)
{
  read_head();      
  install_trans();  // Install all transactions till head
  log.lh.head = 0;
  write_head();     //  Reclaim log
}

void
begin_trans(void)
{
  acquire(&log.lock);
  while (log.intrans) {
    sleep(&log, &log.lock);
  }
  log.intrans = 1;
  release(&log.lock);
}

void
commit_trans(void)
{
  write_head();        // This causes all blocks till log.head to be commited
  install_trans();     // Install all the transactions till head
  log.lh.head = 0; 
  write_head();        // Reclaim log

  acquire(&log.lock);
  log.intrans = 0;
  wakeup(&log);
  release(&log.lock);
}

// Write buffer into the log at log.head and record the block number log.lh.entry, but
// don't write the log header (which would commit the write).
void
log_write(struct buf *b)
{
  int i;

  if (log.lh.head >= LOGSIZE)
    panic("too big a transaction");
  if (!log.intrans)
    panic("write outside of trans");

  cprintf("log_write: %d %d\n", b->sector, log.lh.head);

  for (i = 0; i < log.lh.head; i++) {
    if (log.lh.sector[i] == b->sector)   // log absorbtion?
      break;
  }
  log.lh.sector[i] = b->sector;
  struct buf *lbuf = bread(b->dev, log.start+i+1);
  memmove(lbuf->data, b->data, BSIZE);
  bwrite(lbuf);
  brelse(lbuf);
  if (i == log.lh.head)
    log.lh.head++;
}
