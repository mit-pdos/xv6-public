#include "types.h"
#include "stat.h"
#include "user.h"
#include "mmu.h"
int main(int argc, char *argv[]){
      char *start = sbrk(0);
      sbrk(PGSIZE);
      *start = 100;
      printf(1, "make the page read only\n");
      mprotect(start, 1);
      printf(1, "start = %d\n", *start);
      int pid = fork();
      if (pid == 0){
            printf(1, "-----child-----\n");
            printf(1, "make it read/write on the child\n");
            munprotect(start, 1);
            printf(1, "write on the page\n");
            *start = 5;
            printf(1, "start = %d\n", *start);
            exit();
      }
      else if (pid > 0){
            wait();
            printf(1, "-----parent-----\n");
            printf(1, "write on the page \n'still read only **will trap**'\n");
            *start = 5;
            printf(1, "\nTEST FAILED\n");
            exit();
      }
      exit();
}