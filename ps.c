#include "user.h"

int main(int argc , char* argv[]){
  if(argc!=1){
    printf(1,"Usage: ps\n");
    exit();
  }
  ps();
  exit();
  return 0;
}
