#include "types.h"
#include "stat.h"
#include "user.h"
#include "syscall.h"

int
main(void){
  printf(1, "this should be one: %d\n", dump());
  exit();
}
