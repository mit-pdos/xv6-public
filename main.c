#include "types.h"
#include "multiboot.h"
#include "kernel.h"

extern void inituart(void);
extern void initcga(void);
extern void initconsole(void);
extern void initmp(void);
extern void initlapic(void);
extern void inittrap(void);

void
cmain(void)
{
  inituart();
  initcga();
  initconsole();
  initmp();
  initlapic();
#if 0
  inittrap();
#endif

  cprintf("ncpu %d\n", ncpu);
  panic("end");
}
