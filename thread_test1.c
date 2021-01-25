#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#define NULL (void *)(0)
void f2(void* x,void *y){
printf(1,"I'm 1st thread's child \n");
 thread_join();
exit();
}
//----
void f1(void* x,void *y){
printf(1,"I'm 1st thread \n");
int a = thread_create(&f2, x , y);
 thread_join();
  printf(1, "1st thread's child pid: %d\n",a);
exit();
}


int
main(int argc, char *argv[])
{
 int x=3,y=4;
  printf(1, "RUNNING..\n");
  printf(1, "hi 'm parent\n");
 int a= thread_create(&f1, (void *)&x , (void *)&y);
  thread_join();
  printf(1, "1st thread pid: %d\n",a);
  printf(1, "FINISHING .\n");
  exit();

}
