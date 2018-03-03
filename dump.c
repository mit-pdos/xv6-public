#include "types.h"
#include "stat.h"
#include "user.h"
#include "syscall.h"

int
main(void){
  int i,j;
  int pid = fork();
  if(pid == 0){
    while(1){
      sleep(5);
    }
  }

  int pgm_size =(int) sbrk(0);
  //int pgm_size = 4096;
  void* buff = malloc(pgm_size);
  int *data = (int *)buff;

  //printf(1, "starting system call\n");
  dump(pid, 0, buff, pgm_size);

  for(i=0; i<pgm_size; i=i+sizeof(int)*4){
    printf(1, "0x%x\t", i);
    for(j=0; j<4; j++){
      printf(1, "0x%x\t", *data);
      data++;
    }
    printf(1,"\n");
  }

  exit();
}
