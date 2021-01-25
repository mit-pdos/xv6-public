#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#define NULL (void *)(0)
//summation thread
void s(void* v1,void* v2){
printf(1,"1st thread'c child2 \n");
printf(1,"%d + %d is : %d \n",*(int*)v1, *(int*)v2,*(int*)v1 + *(int*)v2 );

exit();
}
//power thread >> and calls thread that do the summation
void p(void* argm1,void* argm2){
printf(1,"1st thread'c child1 \n");
printf(1,"%d power %d is ",*(int*)argm1,*(int*)argm2);
int i , res =1;
for(i=0; i< *(int*) argm2; i++){
 res = (*(int*) argm1 ) * res;
}
printf(1, ": %d\n",res);
thread_create(&s,argm1,argm2);
thread_join();
exit();
}
//factorial thread
void f(void* arg1,void* arg2){
printf(1,"1st thread \n");
printf(1,"%d factorial is ",*(int*)arg1);
int i , res =1;
for(i=1; i<= *(int*) arg1; i++){
 res = res *i;
}
printf(1, ": %d\n",res);
  thread_create(&p,arg1,arg2);
  thread_join();
exit();
}


int
main(int argc, char *argv[])
{
  
  int x=3,y=4;
  printf(1, "RUNNING..\n");
  thread_create(&f,&x,&y);
  thread_join();
  printf(1, "FINISHING .\n");
  exit();

}


/*#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#define NULL (void *)(0)
struct lock_t *L;
int cnt = 0 ;
void funct(){
for(int i =0 ; i<10000; i++)cnt++;
exit();
}

int
main(int argc, char *argv[])
{
 
  printf(1, "RUNNING..\n");
  lock_init(L);
  lock_acquire(L);
  thread_create(&funct, NULL,NULL);
  lock_release(L);
  thread_join();
  printf(1,"1st increment %d\n",cnt);
  //----
  lock_acquire(L);
  thread_create(&funct, NULL,NULL);
  lock_release(L);
  thread_join();
  printf(1,"2nd increment %d\n",cnt);
  //-----
  lock_acquire(L);
  thread_create(&funct, NULL,NULL);
  lock_release(L);
  thread_join();
  printf(1,"3rd increment %d\n",cnt);
  printf(1, "EXITING  \n");
  exit();

}*/
