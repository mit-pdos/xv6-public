#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int result = ps();
  printf(1, "ps returned <%d>\n", result);
  exit();
}
