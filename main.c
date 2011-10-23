#include "types.h"
#include "multiboot.h"
#include "kernel.h"

extern void inituart(void);
extern void initcga(void);
extern void initconsole(void);
extern void initmp(void);
extern void inittrap(void);

void
cmain(void)
{
  inituart();
  initcga();
  initconsole();
#if 0
  initmp();
  inittrap();
#endif

  panic("end");
}
