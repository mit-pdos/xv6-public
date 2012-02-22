#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "lib.h"
#include "amd64.h"
#include "ipc.hh"
#include "mmu.h"

int
main(int ac, char **av)
{
  char *p = (char*) PGROUNDUP((u64)(sbrk(0) + 0x80000));
  u64 len = 1000*4096;
  u64 chunk = 128*4096; // max vmnode size

#if 0
  char *q = p;
  while(q < p + len){
    u64 x = len - (q - p);
    if(x > chunk)
      x = chunk;
    if(map(q, x) < 0){
      fprintf(1, "map failed\n");
      exit();
    }
    q += x;
  }
#else
  script(p, len, chunk);
#endif

  memset(p, 1, len);

  exit();
}
