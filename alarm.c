#include "types.h"
#include "stat.h"
#include "user.h"

void periodic();

int 
main(int argc,char *argv[])
{
  int i;
  printf(1,"alarmtest starting\n");
  alarm(10,periodic);
  for(i = 0; i < 50*5000000; i++){
    if((i++ % 500000) == 0)
      write(2, ".",1);
  }
  exit();
}

void
periodic()
{
  printf(1,"alarm!\n");
}
