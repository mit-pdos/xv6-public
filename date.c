#include "types.h"
#include "user.h"
#include "date.h"

int
main(int argc, char *argv[])
{
  struct rtcdate r;
  printf(2, "HERE\n");
  if (date(&r)) {
    printf(2, "date failed\n");
    exit();
  }
  //printf(1, "%d: %d: %d\n", r.hour, r.minute, r.second);
  printf(1, "here\n");
  // your code to print the time in any format you like...

  exit();
}
