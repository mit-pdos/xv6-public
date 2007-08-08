// Create a zombie process.

#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  fork();
  exit();
}
