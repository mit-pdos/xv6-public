#include "types.h"
#include "stat.h"
#include "user.h"

int baz() __attribute__((noinline)); 
int baz() {
   int a; 
   a = backtrace();
   return a + uptime();  
}

int bar() __attribute__((noinline)); 
int bar() {
  int b; 
  b = baz(); 
  return b + uptime();
}

int foo() __attribute__((noinline)); 
int foo() {
  int c; 
  c = bar();
  return c + uptime();
}


int
main(int argc, char *argv[])
{
  foo();
  exit();
}