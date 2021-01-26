#include "types.h"
#include "stat.h"
#include "user.h"
#define NULL ((void*)0)

int
main(int argc, char *argv[])
{
  

  int a;
int *pi;     // a pointer to an integer

a = 5;
pi = &a; // pi points to a

a =*pi;
pi=NULL;
printf(1,"%p \n" ,*pi);

   exit();
}