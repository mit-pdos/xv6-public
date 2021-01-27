#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"
#include "fcntl.h"

void spin()
{
  int i = 0;
  int j = 0;
  int k = 0;
  for(i = 0; i < 50; ++i)
    {
      for(j = 0; j < 400000; ++j)
        {
	  k = j % 10;
	  k = k + 1;
	}
    }
}

int
main(int argc, char *argv[]){
  
  int procs = argc - 1;
  if (procs != 3) {
  	printf(1, "useage: lottery_test2 num1 num2 num3");
	exit();
  }
  int numtickets[] = {atoi(argv[1]), atoi(argv[2]), atoi(argv[3])};
  int pid_chds[3];

  pid_chds[0]=getpid();
  settickets(numtickets[0]);

  int i;
  for(i=1;i<3;i++){
    pid_chds[i]=fork();
    if(pid_chds[i]==0){
      while(1) spin();
    }
    else{
      settickets(numtickets[i]);
    }
  }
  // after this loop 
  //  parent->tickets = 30 and will continue the executing the code
  //  first child->tickets = 20 and stuck in the infinity loop calling spin func
  //  second child->tickets = 10 and stuck in the infinity loop calling spin func
    
  struct pstat st;
  int time = 200;
  int ticks[3]={0,0,0};

  printf(1,"pid:%d, pid:%d, pid:%d\n",pid_chds[0],pid_chds[1],pid_chds[2]);
  printf(1,"tickets:%d, tickets:%d, tickets:%d\n",30,20,10);

  while(time--){
      if(getpinfo(&st)!=0){
        printf(1,"check failed: getpinfo\n");
        goto Cleanup;
      }
      
      // search for the parent and childs to get their ticks 
      // and put it in ticks array
      int j;
      int pid;
      for(i=0;i<3;i++){
        pid=pid_chds[i];
        for(j=0;j<NPROC;j++){
	  if(st.pid[j]==pid){
      	    ticks[i]=st.ticks[j];
	  }
        }
      }
     
     // print their ticks
     for(i=0;i<3;i++) printf(1,"%d, ",ticks[i]); 
     printf(1,"\n");
    
     spin();
  }
    
 Cleanup:
  kill(pid_chds[1]);		// kill the 2 child
  kill(pid_chds[2]);
  while(wait() > -1);		// clear their slots in process table
  exit();			// exit parent
}
