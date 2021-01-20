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
  int numtickets[]={20,10,30};
  int pid_chds[3];

  pid_chds[0]=getpid();
  settickets(numtickets[0]);

  int i;
  for(i=1;i<3;i++){
    pid_chds[i]=fork();
    if(pid_chds[i]==0){
      for (;;){
	spin();
      }
    }
    else{
      settickets(numtickets[i]);
    }
  }
    
  struct pstat st;
  int time=0;
  int ticks[3]={0,0,0};

  printf(1,"pid:%d, pid:%d, pid:%d\n",pid_chds[0],pid_chds[1],pid_chds[2]);
  printf(1,"tickets:%d, tickets:%d, tickets:%d\n",30,20,10);

  while(time<200){
    if(getpinfo(&st)!=0){
      printf(1,"check failed: getpinfo\n");
      goto Cleanup;
    }
    
    int j;
    int pid;
    for(i=0;i<3;i++){
      pid=pid_chds[i];
      for(j=0;j<NPROC;j++){
	if(st.pid[j]==pid){
      	  ticks[i]=st.ticks[j];
	  // printf(1,"pid:%d, tickets:%d, ticks:%d\n",pid,st.tickets[j],st.ticks[j]);
	}
      }
    }

   
   for(i=0;i<3;i++){
      printf(1,"%d, ",ticks[i]);
    }
    printf(1,"\n");
    
    spin();
    time++;
  }
    
 Cleanup:
  for (i = 0; pid_chds[i] > 0; i++){
    kill(pid_chds[i]);
  }
  while(wait() > -1);

  exit();
}
