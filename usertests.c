#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char buf[2048];

// simple fork and pipe read/write

void
pipe1(void)
{
  int fds[2], pid;
  int seq = 0, i, n, cc, total;

  pipe(fds);
  pid = fork();
  if(pid == 0){
    close(fds[0]);
    for(n = 0; n < 5; n++){
      for(i = 0; i < 1033; i++)
        buf[i] = seq++;
      if(write(fds[1], buf, 1033) != 1033){
        printf(1, "pipe1 oops 1\n");
        exit();
      }
    }
    exit();
  } else {
    close(fds[1]);
    total = 0;
    cc = 1;
    while((n = read(fds[0], buf, cc)) > 0){
      for(i = 0; i < n; i++){
        if((buf[i] & 0xff) != (seq++ & 0xff)){
          printf(1, "pipe1 oops 2\n");
          return;
        }
      }
      total += n;
      cc = cc * 2;
      if(cc > sizeof(buf))
        cc = sizeof(buf);
    }
    if(total != 5 * 1033)
      printf(1, "pipe1 oops 3\n");
    close(fds[0]);
    wait();
  }
  puts("pipe1 ok\n");
}

// meant to be run w/ at most two CPUs
void
preempt(void)
{
  int pid1, pid2, pid3;
  int pfds[2];

  pid1 = fork();
  if(pid1 == 0)
    for(;;)
      ;
    
  pid2 = fork();
  if(pid2 == 0)
    for(;;)
      ;

  pipe(pfds);
  pid3 = fork();
  if(pid3 == 0){
    close(pfds[0]);
    if(write(pfds[1], "x", 1) != 1)
      printf(1, "preempt write error");
    close(pfds[1]);
    for(;;)
      ;
  }

  close(pfds[1]);
  if(read(pfds[0], buf, sizeof(buf)) != 1){
    printf(1, "preempt read error");
    return;
  }
  close(pfds[0]);
  kill(pid1);
  kill(pid2);
  kill(pid3);
  wait();
  wait();
  wait();
  puts("preempt ok\n");
}

// try to find any races between exit and wait
void
exitwait(void)
{
  int i, pid;

  for(i = 0; i < 100; i++){
    pid = fork();
    if(pid < 0){
      printf(1, "fork failed\n");
      return;
    }
    if(pid){
      if(wait() != pid){
        printf(1, "wait wrong pid\n");
        return;
      }
    } else {
      exit();
    }
  }
  puts("exitwait ok\n");
}

void
mem(void)
{
  void *m1, *m2;

  m1 = 0;
  while ((m2 = malloc(1024)) != 0) {
    printf(1, "malloc %x\n", m2);
    *(char **) m2 = m1;
    m1 = m2;
  }
  while (m1) {
    m2 = *(char **)m1;
    free(m1);
    m1 = m2;
  }
  m1 = malloc(1024*20);
  if (m1 == 0) {
    puts("couldn't allocate mem?!!\n");
    exit();
  }
  free(m1);

  printf(1, "mem ok\n");
}

int
main(int argc, char *argv[])
{
  puts("usertests starting\n");

  mem();
  pipe1();
  preempt();
  exitwait();

  puts("usertests finished\n");
  exit();
}
