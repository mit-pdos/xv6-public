#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"
#include "fcntl.h"

void spin()
{
  int i = 0 ,j = 0 ,k = 0 ;
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
  if (procs != 4) {
  	printf(1, "useage: lottery_test1 Time tickets1 tickets2 tickets3");
	exit();
  }

  int numtickets[] = {atoi(argv[3]), atoi(argv[4]), atoi(argv[2])};
  int pid_chds[3] = {0,0,0};
  int ticks[3] = {0,0,0};
  int time = atoi(argv[1]);
  struct pstat st;
  
  pid_chds[0]=getpid();
  if(settickets(numtickets[0]) < 0 || atoi(argv[3]) <1){
     printf(1, "entered unvalid number\n");
     goto Cleanup;
  }

  for(int i=1;i<3;i++){
    pid_chds[i]=fork();
    if(pid_chds[i]==0){
      while(1) spin();
    }
    else{
      if(settickets(numtickets[i])){
        printf(1, "entered unvalid number\n");
        goto Cleanup;
      }
    }
  }
  // after this loop 
  //  parent->tickets = 30 and will continue the executing the code
  //  first child->tickets = 20 and stuck in the infinity loop calling spin func
  //  second child->tickets = 10 and stuck in the infinity loop calling spin func

  printf(1,"\npid:%d      , pid:%d     , pid:%d   \n",pid_chds[0],pid_chds[1],pid_chds[2]);
  printf(1,"tickets:%d, tickets:%d, tickets:%d\n",atoi(argv[2]),atoi(argv[3]),atoi(argv[4]));

  int t = time ;
  while(time--){
      // get information for all processes in process table
      if(getpinfo(&st)!=0){
        printf(1,"check failed: getpinfo\n");
        goto Cleanup;
      }
      
      // search for the parent and childs to get their ticks 
      // and put it in ticks array
      for(int i=0;i<3;i++){
        for(int j=0;j<NPROC;j++){
      	  if(st.pid[j] == pid_chds[i]){
            	    ticks[i]=st.ticks[j];
      	  }
        }
      }
     
      // print their ticks
      printf(1,"Ticks at %d: ",t-time);
      for(int i=0;i<3;i++) printf(1,"%d | ",ticks[i]); 
      printf(1,"\n");
    
      spin();
  }
    
 Cleanup:
 if(pid_chds[1])kill(pid_chds[1]);		// kill the 2 child
 if(pid_chds[2])kill(pid_chds[2]);
 while(wait() > -1);				// clear their slots in process table
 exit();					// exit parent
}
