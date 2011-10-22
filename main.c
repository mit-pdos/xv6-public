#include "types.h"
#include "multiboot.h"

extern void inituart(void);
extern void initcga(void);

void
cmain(void)
{
  inituart();
  initcga();
}
