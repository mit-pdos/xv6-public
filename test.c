#include "types.h"
#include "user.h"
#include "stat.h"

int
main(int argc, char *argv[])
{
  int rc = fork();
  if(rc < 0){}
  else if(rc == 0)
  {
    int *i = malloc(51400*sizeof(char*));
    for(int j = 0; j < 500000000; ++j)
    {
      *i = 2;
    }
  }
  else
  {
    struct proc_info *pi = malloc(100 * sizeof(struct proc_info));
    pinfo(pi);
    for(int i = 0; i < 10; ++i)
    {
      if(pi[i].pid != 0)
      {
        printf(1, "id=%d\n", pi[i].pid);
        printf(1, "size=%d\n", pi[i].memsize);
      }
    }
    wait();
  }
  exit();
}
