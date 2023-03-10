#include "types.h"
#include "stat.h"
#include "user.h"

struct procInfo arrayOfProcInfo[1024];

int
main(int argc, char *argv[])
{

  int result = ps(1024, arrayOfProcInfo);
  for(int i = 0; i < result; ++i) {
    printf(1, "%s\n", arrayOfProcInfo[i].name);
  }
  printf(1, "ps returned <%d>!\n", result);
  exit();
}
