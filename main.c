#include "types.h"
#include "multiboot.h"
#include "kernel.h"

extern void inituart(void);
extern void initcga(void);
extern void initconsole(void);
extern void initpg(void);
extern void initmp(void);
extern void initlapic(void);
extern void initseg(void);
extern void inittrap(void);
extern void initkalloc(void);
extern void initrcu(void);
extern void initproc(void);
extern void initbio(void);
extern void inituser(void);

void
cmain(void)
{

  inituart();
  initcga();
  initconsole();
  inittrap();
  initpg();
  initmp();
  initlapic();
  initseg();

  initkalloc();
  initrcu();       // initialize rcu module
  initproc();      // process table
  initbio();       // buffer cache
#if 0
  fileinit();      // file table
  iinit();         // inode cache
  ideinit();       // disk
#endif

  inituser();      // first user process
  
  cprintf("ncpu %d\n", ncpu);
  panic("end");
}
