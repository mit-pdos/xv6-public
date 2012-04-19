#include "types.h"
#include "kernel.hh"
#include "mmu.h"
#include "amd64.h"
#include "spinlock.h"
#include "condvar.h"
#include "fs.h"
#include "file.hh"
#include "major.h"

extern char cmdline[];

static int
cmdlineread(struct inode *inode, char *dst, u32 off, u32 n)
{
  u32 cc;

  if (off >= strlen(cmdline))
    return 0;

  cc = MIN(n, strlen(cmdline)-off);
  memcpy(dst, &cmdline[off], cc);
  return cc;
}

void
initcmdline(void)
{
  if (VERBOSE)
    cprintf("cmdline: %s\n", cmdline);

  devsw[MAJ_CMDLINE].write = nullptr;
  devsw[MAJ_CMDLINE].read = cmdlineread;
}
