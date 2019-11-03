#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

main(int argc, char *argv[]){
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
    
    printf("Wait Time = %d\n",wtime);
    printf("Run Time = %d\n",rtime);
    printf("Status: %d\n",status);
    exit();
    
}
