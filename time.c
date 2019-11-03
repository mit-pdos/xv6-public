#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

int main(int argc, char *argv[]){
  int pid;
  int status, wtime, rtime;
  pid = fork();
  if(pid == 0)
  {
    exec(argv[1],argv);
    printf(1,"exec %s failed\n",argv[1]);
  }
  
  else{
    status = waitx(&wtime, &rtime);
    } 
    
    printf(1,"Wait Time = %d\n",wtime);
    printf(1,"Run Time = %d\n",rtime);
    printf(1,"Status: %d\n",status);
    exit(0);
    
}
