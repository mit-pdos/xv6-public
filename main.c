#include "types.h"
#include "multiboot.h"

extern void uartinit(void);

void
cmain(void)
{
  uartinit();
}
