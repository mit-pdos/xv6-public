#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#define NULL (void *)(0)
struct lock_t *L ;

int cnt = 0 ; //global variable must be put in critical region

void f1(){
   for(int i=0;i<10;i++){
	int pid =getpid();
	//critical region
	lock_acquire(L);
	printf(1,"PID : %d -->cnt: %d \n",pid,cnt++);
	lock_release(L);
   }
   exit();
}
//without applying the locks , a race condition may happen.
void f2(){
   for(int i=0;i<10;i++){
	int pid =getpid();
	
	printf(1,"PID : %d -->cnt: %d \n",pid,cnt++);
   }
   exit();
}


int
main(int argc, char *argv[])
{
  L = malloc(16);

  printf(1, "RUNNING..\n");
  lock_init(L);
  thread_create(&f1, NULL,NULL);
  thread_create(&f1, NULL,NULL);
  thread_create(&f1, NULL,NULL);
  thread_join();
  thread_join();
  thread_join();
  
  /*
  thread_create(&f2, NULL,NULL);
  thread_create(&f2, NULL,NULL);
  thread_create(&f2, NULL,NULL);
  thread_join();
  thread_join();
  thread_join();
 */
  printf(1,"\n The Total Count :: %d\n",cnt);
  printf(1, "EXITING  \n");
  
  exit();

}

