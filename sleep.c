#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  int i;

  for (i = 0; i < 10000000; i++) {
    sleep(1);
  }
}
