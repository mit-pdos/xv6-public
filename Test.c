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
  float i;
  float uselessvariable =0;

  for(k = 0; k< 5; k++){

    pid = fork();
    // Error Check
    if(pid<0){
      printf(1,"Failed to fork at iteration %d\n",k);
      exit();
    }
    // For child process:
    else if(pid == 0){
      for(i=0;i<1000000000;i+=1){
        uselessvariable = uselessvariable+3.14*2.687;

        // sleep(10);
      }
      exit();
    }

  }
  for(k=0;k<10;k++){
    wait();
  }
  exit();
}
