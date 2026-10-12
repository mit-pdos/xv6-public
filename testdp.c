#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  int pages;
  char *buf;
  int i, pid, s;

  printf(1, "test: demand paging\n");
  pages = 1500;
  buf = sbrk(pages * 4096);
  for(i = 0; i < pages; i += 128){
    buf[i * 4096] = 'A';
  }

  printf(1, "test: eviction + swapin\n");
  pages = 70000;
  buf = sbrk(pages * 4096);
  for(i = 0; i < pages; i++){
    buf[i * 4096] = i;
  }
  s = 0;
  for(i = 0; i < pages; i++){
    s += buf[i * 4096];
  }
  printf(1, "sum = %d\n", s);

  printf(1, "test: fork with swapped pages\n");
  pages = 70000;
  buf = sbrk(pages * 4096);
  for(i = 0; i < pages; i++){
    buf[i * 4096] = i;
  }
  pid = fork();
  if(pid == 0){
    s = 0;
    for(i = 0; i < pages; i++){
      s += buf[i * 4096];
    }
    printf(1, "child sum = %d\n", s);
    exit();
  }
  wait();

  printf(1, "test: child modifies swapped page\n");
  pages = 70000;
  buf = sbrk(pages * 4096);
  for(i = 0; i < pages; i++){
    buf[i * 4096] = 1;
  }
  pid = fork();
  if(pid == 0){
    buf[60000 * 4096] = 5;
    printf(1, "child sees %d\n", buf[60000 * 4096]);
    exit();
  }
  wait();
  printf(1, "parent sees %d\n", buf[60000 * 4096]);

  printf(1, "test: stress alloc/free\n");
  for(i = 0; i < 200; i++){
    buf = sbrk(4096 * 300);
    int j;
    for(j = 0; j < 300; j++){
      buf[j * 4096] = i + j;
    }
    sbrk(-4096 * 300);
  }

  printf(1, "test: OOM boundary\n");
  while(1){
    buf = sbrk(4096);
    if(buf == (char*)-1){
      printf(1, "oom reached\n");
      break;
    }
    *buf = 1;
  }

  exit();
}
