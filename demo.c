#include "types.h"
#include "stat.h"
#include "user.h"


char buf[512];

int
main(int argc, char *argv[])
{
  int fd, i, st;
    if(argc <= 1){
    //cat(0);
    exit();
  }
  for(i = 1; i < argc; i++){
    if((fd = open(argv[i], 0)) < 0){
      printf(1, "cat: cannot open %s\n", argv[i]);
      exit();
    }
    int request=1;
    int status = flockwr(fd, &st,request);
    if(status==0)
    {
       printf(1, "file locking status is: %d\n", st);
    }
    else
        printf(1, "status error \n");
    int writeable = write(fd,"123",3);
    printf(1, "writeable: %d\n", writeable);
    close(fd);
  }
  exit();
}
