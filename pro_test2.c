#include "types.h"
#include "stat.h"
#include "user.h"
#include "mmu.h"
int main(int argc, char *argv[]){
      char *first = sbrk(0);
      sbrk(PGSIZE << 1);
      char *second = first + PGSIZE;
      *first = 10;
      *second = 20;
      printf(1, "make the 2 pages read only\n");
      mprotect(first, 2);
      printf(1, "make the second page read/write\n");
      munprotect(second, 1);
      printf(1, "first = %d\nsecond = %d\n", *first,*second);
      int pid = fork();
      if (pid == 0){
            printf(1, "-----child-----\n");
            printf(1, "make the first page read/write on the child\n");
            munprotect(first, 1);
            printf(1, "write on both of the pages\n");
            *first = 1;
            *second  = 2;
            printf(1, "first = %d\nsecond = %d\n", *first,*second);
            exit();
      }
      else if (pid > 0){
            wait();
            printf(1, "-----parent-----\n");
            printf(1, "write on the second page\n");
            *second = 22;
            printf(1, "first = %d\nsecond = %d\n", *first,*second);
            printf(1, "write on the first page \n'still read only **will trap**'\n");
            *first = 11;
            printf(1, "\nTEST FAILED\n");
            exit();
      }
      exit();
}
