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

// two processes write to the same file descriptor
// is the offset shared? does inode locking work?
void
sharedfd()
{
  int fd, pid, i, n, nc, np;
  char buf[10];

  unlink("sharedfd");
  fd = open("sharedfd", O_CREATE|O_RDWR);
  if(fd < 0){
    printf(1, "usertests: cannot open sharedfd for writing");
    return;
  }
  pid = fork();
  memset(buf, pid==0?'c':'p', sizeof(buf));
  for(i = 0; i < 100; i++){
    if(write(fd, buf, sizeof(buf)) != sizeof(buf)){
      printf(1, "usertests: write sharedfd failed\n");
      break;
    }
  }
  if(pid == 0)
    exit();
  else
    wait();
  close(fd);
  fd = open("sharedfd", 0);
  if(fd < 0){
    printf(1, "usertests: cannot open sharedfd for reading\n");
    return;
  }
  nc = np = 0;
  while((n = read(fd, buf, sizeof(buf))) > 0){
    for(i = 0; i < sizeof(buf); i++){
      if(buf[i] == 'c')
        nc++;
      if(buf[i] == 'p')
        np++;
    }
  }
  close(fd);
  if(nc == 1000 && np == 1000)
    printf(1, "sharedfd ok\n");
  else
    printf(1, "sharedfd oops %d %d\n", nc, np);
}

// two processes write two different files at the same
// time, to test block allocation.
void
twofiles()
{
  int fd, pid, i, j, n, total;
  char *fname;

  unlink("f1");
  unlink("f2");

  pid = fork();
  if(pid < 0){
    puts("fork failed\n");
    return;
  }

  fname = pid ? "f1" : "f2";
  fd = open(fname, O_CREATE | O_RDWR);
  if(fd < 0){
    puts("create failed\n");
    exit();
  }

  memset(buf, pid?'p':'c', 512);
  for(i = 0; i < 12; i++){
    if((n = write(fd, buf, 500)) != 500){
      printf(1, "write failed %d\n", n);
      exit();
    }
  }
  close(fd);
  if(pid)
    wait();
  else
    exit();

  for(i = 0; i < 2; i++){
    fd = open(i?"f1":"f2", 0);
    total = 0;
    while((n = read(fd, buf, sizeof(buf))) > 0){
      for(j = 0; j < n; j++){
        if(buf[j] != (i?'p':'c')){
          puts("wrong char\n");
          exit();
        }
      }
      total += n;
    }
    close(fd);
    if(total != 12*500){
      printf(1, "wrong length %d\n", total);
      exit();
    }
  }

  puts("twofiles ok\n");
}

int
main(int argc, char *argv[])
{
  puts("usertests starting\n");

  twofiles();
  sharedfd();
  pipe1();
  preempt();
  exitwait();

  puts("usertests finished\n");
  exit();
}
