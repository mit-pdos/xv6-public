#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

int main(int argc, char* argv[]){
  #ifdef FCFS
    printf(1,"FCFS\n");
  #endif

  int pid,k;
  int uselessvariable =0;

  for(k = 0; k< 10; k++){

    pid = fork();
    // Error Check
    if(pid<0){
      printf(1,"Failed to fork at iteration %d\n",k);
      exit();
    }
    // For child process:
    else if(pid == 0){
      // printf(1,"PID: %d\n",getpid());
      // for(i=0;i<10000000;i+=1){
      //   uselessvariable++;
        // sleep(5);

      // }
      exec(argv[1],argv+1);
      printf(1,"%d\n",uselessvariable);
      exit();
    }

  }
  for(k=0;k<10;k++){
    wait();
  }
  exit();
}
