//
// tests for copy-on-write fork() assignment.
//

#include "kernel/types.h"
#include "kernel/memlayout.h"
#include "user/user.h"

// allocate more than half of physical memory,
// then fork. this will fail in the default
// kernel, which does not support copy-on-write.
void
simpletest()
{
  uint64 phys_size = PHYSTOP - KERNBASE;
  int sz = (phys_size / 3) * 2;

  printf(1, "simple: ");
  
  char *p = sbrk(sz);
  if(p == (char*)0xffffffffffffffffL){
    printf(1, "sbrk(%d) failed\n", sz);
    exit();
  }

  for(char *q = p; q < p + sz; q += 4096){
    *(int*)q = getpid();
  }

  int pid = fork();
  if(pid < 0){
    printf(1, "fork() failed\n");
    exit();
  }

  if(pid == 0)
    exit();

  wait();

  if(sbrk(-sz) == (char*)0xffffffffffffffffL){
    printf(1, "sbrk(-%d) failed\n", sz);
    exit();
  }

  printf(1, "ok\n");
}

// three processes all write COW memory.
// this causes more than half of physical memory
// to be allocated, so it also checks whether
// copied pages are freed.
void
threetest()
{
  uint64 phys_size = PHYSTOP - KERNBASE;
  int sz = phys_size / 4;
  int pid1, pid2;

  printf(1, "three: ");
  
  char *p = sbrk(sz);
  if(p == (char*)0xffffffffffffffffL){
    printf(1, "sbrk(%d) failed\n", sz);
    exit();
  }

  pid1 = fork();
  if(pid1 < 0){
    printf(1, "fork failed\n");
    exit();
  }
  if(pid1 == 0){
    pid2 = fork();
    if(pid2 < 0){
      printf(1, "fork failed");
      exit();
    }
    if(pid2 == 0){
      for(char *q = p; q < p + (sz/5)*4; q += 4096){
        *(int*)q = getpid();
      }
      for(char *q = p; q < p + (sz/5)*4; q += 4096){
        if(*(int*)q != getpid()){
          printf(1, "wrong content\n");
          exit();
        }
      }
      exit();
    }
    for(char *q = p; q < p + (sz/2); q += 4096){
      *(int*)q = 9999;
    }
    exit();
  }

  for(char *q = p; q < p + sz; q += 4096){
    *(int*)q = getpid();
  }

  wait();

  sleep(1);

  for(char *q = p; q < p + sz; q += 4096){
    if(*(int*)q != getpid()){
      printf(1, "wrong content\n");
      exit();
    }
  }

  if(sbrk(-sz) == (char*)0xffffffffffffffffL){
    printf(1, "sbrk(-%d) failed\n", sz);
    exit();
  }

  printf(1, "ok\n");
}

char junk1[4096];
int fds[2];
char junk2[4096];
char buf[4096];
char junk3[4096];

// test whether copyout() simulates COW faults.
void
filetest()
{
  int parent = getpid();
  
  printf(1, "file: ");
  
  buf[0] = 99;

  for(int i = 0; i < 4; i++){
    if(pipe(fds) != 0){
      printf(1, "pipe() failed\n");
      exit();
    }
    int pid = fork();
    if(pid < 0){
      printf(1, "fork failed\n");
      exit();
    }
    if(pid == 0){
      sleep(1);
      if(read(fds[0], buf, sizeof(i)) != sizeof(i)){
        printf(1, "read failed\n");
        kill(parent);
        exit();
      }
      sleep(1);
      int j = *(int*)buf;
      if(j != i){
        printf(1, "read the wrong value\n");
        kill(parent);
        exit();
      }
      exit();
    }
    if(write(fds[1], &i, sizeof(i)) != sizeof(i)){
      printf(1, "write failed\n");
      exit();
    }
  }

  for(int i = 0; i < 4; i++)
    wait();

  if(buf[0] != 99){
    printf(1, "child overwrote parent\n");
    exit();
  }

  printf(1, "ok\n");
}

int
main(int argc, char *argv[])
{
  simpletest();

  // check that the first simpletest() freed the physical memory.
  simpletest();

  threetest();
  threetest();
  threetest();

  filetest();

  printf(1, "ALL COW TESTS PASSED\n");

  exit();
}
