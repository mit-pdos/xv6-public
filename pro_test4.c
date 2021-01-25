#include "types.h"
#include "stat.h"
#include "user.h"
#include "mmu.h"
int toInt(char *s){
      int ret = 0;
      int sign = 0;
      if (*s == '-'){
            s++;
            sign = 1;
      }
      while (*s){
            if (*s > '9' || *s < '0'){
                  printf(1, "parameters are numbers only\n");
                  return -1;
            }
            ret = ret * 10 + (*s - '0');
            s++;
      }
      if (sign)
            ret *= -1;
      return ret;
}
int abs(int x) { return x < 0 ? -x : x; }
int main(int argc, char *argv[]){
      if (argc != 3){
            printf(1, "enter the parameters of the test\npro_test4 offset len\nex: pro_test4 0 4\n");
            exit();
      }

      int offset = toInt(argv[1]);
      int len = toInt(argv[2]);

      char *start = sbrk(0);
      sbrk(PGSIZE * abs(len)); // to show the error that will happent if we passed neg len

      *(start+offset) = 10;
      int x = mprotect(start + offset, len);

      if (x == 0)
            printf(1, "protection succeded i will trap after changing the value\n");
      else 
            printf(1, "protection failed\n");

      if(len){
            printf(1, "changing the value\n");
            printf(1,"val = %d\n",*start);
            *(start+offset) = 5;
            printf(1, "the value has been changed!\n");
            printf(1,"val = %d\n",*start);
      }
      exit();
}
