#include "types.h"
#include "user.h"

void
abort(void)
{
  char* p = (char*) 1;
  *p = 1;
  exit();
}

int
dl_iterate_phdr(void)
{
  return -1;
}
