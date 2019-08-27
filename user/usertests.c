#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/syscall.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"

#define BUFSZ  (MAXOPBLOCKS+2)*BSIZE

char buf[BUFSZ];
char name[3];
char *echoargv[] = { "echo", "ALL", "TESTS", "PASSED", 0 };

// does chdir() call iput(p->cwd) in a transaction?
void
iputtest(void)
{
  printf("iput test\n");

  if(mkdir("iputdir") < 0){
    printf("mkdir failed\n");
    exit();
  }
  if(chdir("iputdir") < 0){
    printf("chdir iputdir failed\n");
    exit();
  }
  if(unlink("../iputdir") < 0){
    printf("unlink ../iputdir failed\n");
    exit();
  }
  if(chdir("/") < 0){
    printf("chdir / failed\n");
    exit();
  }
  printf("iput test ok\n");
}

// does exit() call iput(p->cwd) in a transaction?
void
exitiputtest(void)
{
  int pid;

  printf("exitiput test\n");

  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    exit();
  }
  if(pid == 0){
    if(mkdir("iputdir") < 0){
      printf("mkdir failed\n");
      exit();
    }
    if(chdir("iputdir") < 0){
      printf("child chdir failed\n");
      exit();
    }
    if(unlink("../iputdir") < 0){
      printf("unlink ../iputdir failed\n");
      exit();
    }
    exit();
  }
  wait();
  printf("exitiput test ok\n");
}

// does the error path in open() for attempt to write a
// directory call iput() in a transaction?
// needs a hacked kernel that pauses just after the namei()
// call in sys_open():
//    if((ip = namei(path)) == 0)
//      return -1;
//    {
//      int i;
//      for(i = 0; i < 10000; i++)
//        yield();
//    }
void
openiputtest(void)
{
  int pid;

  printf("openiput test\n");
  if(mkdir("oidir") < 0){
    printf("mkdir oidir failed\n");
    exit();
  }
  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    exit();
  }
  if(pid == 0){
    int fd = open("oidir", O_RDWR);
    if(fd >= 0){
      printf("open directory for write succeeded\n");
      exit();
    }
    exit();
  }
  sleep(1);
  if(unlink("oidir") != 0){
    printf("unlink failed\n");
    exit();
  }
  wait();
  printf("openiput test ok\n");
}

// simple file system tests

void
opentest(void)
{
  int fd;

  printf("open test\n");
  fd = open("echo", 0);
  if(fd < 0){
    printf("open echo failed!\n");
    exit();
  }
  close(fd);
  fd = open("doesnotexist", 0);
  if(fd >= 0){
    printf("open doesnotexist succeeded!\n");
    exit();
  }
  printf("open test ok\n");
}

void
writetest(void)
{
  int fd;
  int i;
  enum { N=100, SZ=10 };
  
  printf("small file test\n");
  fd = open("small", O_CREATE|O_RDWR);
  if(fd >= 0){
    printf("creat small succeeded; ok\n");
  } else {
    printf("error: creat small failed!\n");
    exit();
  }
  for(i = 0; i < N; i++){
    if(write(fd, "aaaaaaaaaa", SZ) != SZ){
      printf("error: write aa %d new file failed\n", i);
      exit();
    }
    if(write(fd, "bbbbbbbbbb", SZ) != SZ){
      printf("error: write bb %d new file failed\n", i);
      exit();
    }
  }
  printf("writes ok\n");
  close(fd);
  fd = open("small", O_RDONLY);
  if(fd >= 0){
    printf("open small succeeded ok\n");
  } else {
    printf("error: open small failed!\n");
    exit();
  }
  i = read(fd, buf, N*SZ*2);
  if(i == N*SZ*2){
    printf("read succeeded ok\n");
  } else {
    printf("read failed\n");
    exit();
  }
  close(fd);

  if(unlink("small") < 0){
    printf("unlink small failed\n");
    exit();
  }
  printf("small file test ok\n");
}

void
writetest1(void)
{
  int i, fd, n;

  printf("big files test\n");

  fd = open("big", O_CREATE|O_RDWR);
  if(fd < 0){
    printf("error: creat big failed!\n");
    exit();
  }

  for(i = 0; i < MAXFILE; i++){
    ((int*)buf)[0] = i;
    if(write(fd, buf, BSIZE) != BSIZE){
      printf("error: write big file failed\n", i);
      exit();
    }
  }

  close(fd);

  fd = open("big", O_RDONLY);
  if(fd < 0){
    printf("error: open big failed!\n");
    exit();
  }

  n = 0;
  for(;;){
    i = read(fd, buf, BSIZE);
    if(i == 0){
      if(n == MAXFILE - 1){
        printf("read only %d blocks from big", n);
        exit();
      }
      break;
    } else if(i != BSIZE){
      printf("read failed %d\n", i);
      exit();
    }
    if(((int*)buf)[0] != n){
      printf("read content of block %d is %d\n",
             n, ((int*)buf)[0]);
      exit();
    }
    n++;
  }
  close(fd);
  if(unlink("big") < 0){
    printf("unlink big failed\n");
    exit();
  }
  printf("big files ok\n");
}

void
createtest(void)
{
  int i, fd;
  enum { N=52 };
  
  printf("many creates, followed by unlink test\n");

  name[0] = 'a';
  name[2] = '\0';
  for(i = 0; i < N; i++){
    name[1] = '0' + i;
    fd = open(name, O_CREATE|O_RDWR);
    close(fd);
  }
  name[0] = 'a';
  name[2] = '\0';
  for(i = 0; i < N; i++){
    name[1] = '0' + i;
    unlink(name);
  }
  printf("many creates, followed by unlink; ok\n");
}

void dirtest(void)
{
  printf("mkdir test\n");

  if(mkdir("dir0") < 0){
    printf("mkdir failed\n");
    exit();
  }

  if(chdir("dir0") < 0){
    printf("chdir dir0 failed\n");
    exit();
  }

  if(chdir("..") < 0){
    printf("chdir .. failed\n");
    exit();
  }

  if(unlink("dir0") < 0){
    printf("unlink dir0 failed\n");
    exit();
  }
  printf("mkdir test ok\n");
}

void
exectest(void)
{
  printf("exec test\n");
  if(exec("echo", echoargv) < 0){
    printf("exec echo failed\n");
    exit();
  }
}

// simple fork and pipe read/write

void
pipe1(void)
{
  int fds[2], pid;
  int seq, i, n, cc, total;
  enum { N=5, SZ=1033 };
  
  if(pipe(fds) != 0){
    printf("pipe() failed\n");
    exit();
  }
  pid = fork();
  seq = 0;
  if(pid == 0){
    close(fds[0]);
    for(n = 0; n < N; n++){
      for(i = 0; i < SZ; i++)
        buf[i] = seq++;
      if(write(fds[1], buf, SZ) != SZ){
        printf("pipe1 oops 1\n");
        exit();
      }
    }
    exit();
  } else if(pid > 0){
    close(fds[1]);
    total = 0;
    cc = 1;
    while((n = read(fds[0], buf, cc)) > 0){
      for(i = 0; i < n; i++){
        if((buf[i] & 0xff) != (seq++ & 0xff)){
          printf("pipe1 oops 2\n");
          return;
        }
      }
      total += n;
      cc = cc * 2;
      if(cc > sizeof(buf))
        cc = sizeof(buf);
    }
    if(total != N * SZ){
      printf("pipe1 oops 3 total %d\n", total);
      exit();
    }
    close(fds[0]);
    wait();
  } else {
    printf("fork() failed\n");
    exit();
  }
  printf("pipe1 ok\n");
}

// meant to be run w/ at most two CPUs
void
preempt(void)
{
  int pid1, pid2, pid3;
  int pfds[2];

  printf("preempt: ");
  pid1 = fork();
  if(pid1 < 0) {
    printf("fork failed");
    exit();
  }
  if(pid1 == 0)
    for(;;)
      ;

  pid2 = fork();
  if(pid2 < 0) {
    printf("fork failed\n");
    exit();
  }
  if(pid2 == 0)
    for(;;)
      ;

  pipe(pfds);
  pid3 = fork();
  if(pid3 < 0) {
     printf("fork failed\n");
     exit();
  }
  if(pid3 == 0){
    close(pfds[0]);
    if(write(pfds[1], "x", 1) != 1)
      printf("preempt write error");
    close(pfds[1]);
    for(;;)
      ;
  }

  close(pfds[1]);
  if(read(pfds[0], buf, sizeof(buf)) != 1){
    printf("preempt read error");
    return;
  }
  close(pfds[0]);
  printf("kill... ");
  kill(pid1);
  kill(pid2);
  kill(pid3);
  printf("wait... ");
  wait();
  wait();
  wait();
  printf("preempt ok\n");
}

// try to find any races between exit and wait
void
exitwait(void)
{
  int i, pid;

  printf("exitwait test\n");

  for(i = 0; i < 100; i++){
    pid = fork();
    if(pid < 0){
      printf("fork failed\n");
      exit();
    }
    if(pid){
      if(wait() != pid){
        printf("wait wrong pid\n");
        exit();
      }
    } else {
      exit();
    }
  }
  printf("exitwait ok\n");
}

// try to find races in the reparenting
// code that handles a parent exiting
// when it still has live children.
void
reparent(void)
{
  int master_pid = getpid();
  
  printf("reparent test\n");

  for(int i = 0; i < 200; i++){
    int pid = fork();
    if(pid < 0){
      printf("fork failed\n");
      exit();
    }
    if(pid){
      if(wait() != pid){
        printf("wait wrong pid\n");
        exit();
      }
    } else {
      int pid2 = fork();
      if(pid2 < 0){
        printf("fork failed\n");
        kill(master_pid);
        exit();
      }
      if(pid2 == 0){
        exit();
      } else {
        exit();
      }
    }
  }
  printf("reparent ok\n");
}

// what if two children exit() at the same time?
void
twochildren(void)
{
  printf("twochildren test\n");

  for(int i = 0; i < 1000; i++){
    int pid1 = fork();
    if(pid1 < 0){
      printf("fork failed\n");
      exit();
    }
    if(pid1 == 0){
      exit();
    } else {
      int pid2 = fork();
      if(pid2 < 0){
        printf("fork failed\n");
        exit();
      }
      if(pid2 == 0){
        exit();
      } else {
        wait();
        wait();
      }
    }
  }
  printf("twochildren ok\n");
}

// concurrent forks to try to expose locking bugs.
void
forkfork(void)
{
  int ppid = getpid();
  enum { N=2 };
  
  printf("forkfork test\n");

  for(int i = 0; i < N; i++){
    int pid = fork();
    if(pid < 0){
      printf("fork failed");
      exit();
    }
    if(pid == 0){
      for(int j = 0; j < 200; j++){
        int pid1 = fork();
        if(pid1 < 0){
          printf("fork failed\n");
          kill(ppid);
          exit();
        }
        if(pid1 == 0){
          exit();
        }
        wait();
      }
      exit();
    }
  }

  for(int i = 0; i < N; i++){
    wait();
  }

  printf("forkfork ok\n");
}

void
forkforkfork(void)
{
  printf("forkforkfork test\n");

  unlink("stopforking");

  int pid = fork();
  if(pid < 0){
    printf("fork failed");
    exit();
  }
  if(pid == 0){
    while(1){
      int fd = open("stopforking", 0);
      if(fd >= 0){
        exit();
      }
      if(fork() < 0){
        close(open("stopforking", O_CREATE|O_RDWR));
      }
    }

    exit();
  }

  sleep(20); // two seconds
  close(open("stopforking", O_CREATE|O_RDWR));
  wait();
  sleep(10); // one second

  printf("forkforkfork ok\n");
}

void
mem(void)
{
  void *m1, *m2;
  int pid, ppid;

  printf("mem test\n");
  ppid = getpid();
  if((pid = fork()) == 0){
    m1 = 0;
    while((m2 = malloc(10001)) != 0){
      *(char**)m2 = m1;
      m1 = m2;
    }
    while(m1){
      m2 = *(char**)m1;
      free(m1);
      m1 = m2;
    }
    m1 = malloc(1024*20);
    if(m1 == 0){
      printf("couldn't allocate mem?!!\n");
      kill(ppid);
      exit();
    }
    free(m1);
    printf("mem ok\n");
    exit();
  } else {
    wait();
  }
}

// More file system tests

// two processes write to the same file descriptor
// is the offset shared? does inode locking work?
void
sharedfd(void)
{
  int fd, pid, i, n, nc, np;
  enum { N = 1000, SZ=10};
  char buf[SZ];

  printf("sharedfd test\n");

  unlink("sharedfd");
  fd = open("sharedfd", O_CREATE|O_RDWR);
  if(fd < 0){
    printf("fstests: cannot open sharedfd for writing");
    return;
  }
  pid = fork();
  memset(buf, pid==0?'c':'p', sizeof(buf));
  for(i = 0; i < N; i++){
    if(write(fd, buf, sizeof(buf)) != sizeof(buf)){
      printf("fstests: write sharedfd failed\n");
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
    printf("fstests: cannot open sharedfd for reading\n");
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
  unlink("sharedfd");
  if(nc == N*SZ && np == N*SZ){
    printf("sharedfd ok\n");
  } else {
    printf("sharedfd oops %d %d\n", nc, np);
    exit();
  }
}

// four processes write different files at the same
// time, to test block allocation.
void
fourfiles(void)
{
  int fd, pid, i, j, n, total, pi;
  char *names[] = { "f0", "f1", "f2", "f3" };
  char *fname;
  enum { N=12, NCHILD=4, SZ=500 };
  
  printf("fourfiles test\n");

  for(pi = 0; pi < NCHILD; pi++){
    fname = names[pi];
    unlink(fname);

    pid = fork();
    if(pid < 0){
      printf("fork failed\n");
      exit();
    }

    if(pid == 0){
      fd = open(fname, O_CREATE | O_RDWR);
      if(fd < 0){
        printf("create failed\n");
        exit();
      }

      memset(buf, '0'+pi, SZ);
      for(i = 0; i < N; i++){
        if((n = write(fd, buf, SZ)) != SZ){
          printf("write failed %d\n", n);
          exit();
        }
      }
      exit();
    }
  }

  for(pi = 0; pi < NCHILD; pi++){
    wait();
  }

  for(i = 0; i < NCHILD; i++){
    fname = names[i];
    fd = open(fname, 0);
    total = 0;
    while((n = read(fd, buf, sizeof(buf))) > 0){
      for(j = 0; j < n; j++){
        if(buf[j] != '0'+i){
          printf("wrong char\n");
          exit();
        }
      }
      total += n;
    }
    close(fd);
    if(total != N*SZ){
      printf("wrong length %d\n", total);
      exit();
    }
    unlink(fname);
  }

  printf("fourfiles ok\n");
}

// four processes create and delete different files in same directory
void
createdelete(void)
{
  enum { N = 20, NCHILD=4 };
  int pid, i, fd, pi;
  char name[32];

  printf("createdelete test\n");

  for(pi = 0; pi < NCHILD; pi++){
    pid = fork();
    if(pid < 0){
      printf("fork failed\n");
      exit();
    }

    if(pid == 0){
      name[0] = 'p' + pi;
      name[2] = '\0';
      for(i = 0; i < N; i++){
        name[1] = '0' + i;
        fd = open(name, O_CREATE | O_RDWR);
        if(fd < 0){
          printf("create failed\n");
          exit();
        }
        close(fd);
        if(i > 0 && (i % 2 ) == 0){
          name[1] = '0' + (i / 2);
          if(unlink(name) < 0){
            printf("unlink failed\n");
            exit();
          }
        }
      }
      exit();
    }
  }

  for(pi = 0; pi < NCHILD; pi++){
    wait();
  }

  name[0] = name[1] = name[2] = 0;
  for(i = 0; i < N; i++){
    for(pi = 0; pi < NCHILD; pi++){
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      fd = open(name, 0);
      if((i == 0 || i >= N/2) && fd < 0){
        printf("oops createdelete %s didn't exist\n", name);
        exit();
      } else if((i >= 1 && i < N/2) && fd >= 0){
        printf("oops createdelete %s did exist\n", name);
        exit();
      }
      if(fd >= 0)
        close(fd);
    }
  }

  for(i = 0; i < N; i++){
    for(pi = 0; pi < NCHILD; pi++){
      name[0] = 'p' + i;
      name[1] = '0' + i;
      unlink(name);
    }
  }

  printf("createdelete ok\n");
}

// can I unlink a file and still read it?
void
unlinkread(void)
{
  enum { SZ = 5 };
  int fd, fd1;

  printf("unlinkread test\n");
  fd = open("unlinkread", O_CREATE | O_RDWR);
  if(fd < 0){
    printf("create unlinkread failed\n");
    exit();
  }
  write(fd, "hello", SZ);
  close(fd);

  fd = open("unlinkread", O_RDWR);
  if(fd < 0){
    printf("open unlinkread failed\n");
    exit();
  }
  if(unlink("unlinkread") != 0){
    printf("unlink unlinkread failed\n");
    exit();
  }

  fd1 = open("unlinkread", O_CREATE | O_RDWR);
  write(fd1, "yyy", 3);
  close(fd1);

  if(read(fd, buf, sizeof(buf)) != SZ){
    printf("unlinkread read failed");
    exit();
  }
  if(buf[0] != 'h'){
    printf("unlinkread wrong data\n");
    exit();
  }
  if(write(fd, buf, 10) != 10){
    printf("unlinkread write failed\n");
    exit();
  }
  close(fd);
  unlink("unlinkread");
  printf("unlinkread ok\n");
}

void
linktest(void)
{
  enum { SZ = 5 };
  int fd;

  printf("linktest\n");

  unlink("lf1");
  unlink("lf2");

  fd = open("lf1", O_CREATE|O_RDWR);
  if(fd < 0){
    printf("create lf1 failed\n");
    exit();
  }
  if(write(fd, "hello", SZ) != SZ){
    printf("write lf1 failed\n");
    exit();
  }
  close(fd);

  if(link("lf1", "lf2") < 0){
    printf("link lf1 lf2 failed\n");
    exit();
  }
  unlink("lf1");

  if(open("lf1", 0) >= 0){
    printf("unlinked lf1 but it is still there!\n");
    exit();
  }

  fd = open("lf2", 0);
  if(fd < 0){
    printf("open lf2 failed\n");
    exit();
  }
  if(read(fd, buf, sizeof(buf)) != SZ){
    printf("read lf2 failed\n");
    exit();
  }
  close(fd);

  if(link("lf2", "lf2") >= 0){
    printf("link lf2 lf2 succeeded! oops\n");
    exit();
  }

  unlink("lf2");
  if(link("lf2", "lf1") >= 0){
    printf("link non-existant succeeded! oops\n");
    exit();
  }

  if(link(".", "lf1") >= 0){
    printf("link . lf1 succeeded! oops\n");
    exit();
  }

  printf("linktest ok\n");
}

// test concurrent create/link/unlink of the same file
void
concreate(void)
{
  enum { N = 40 };
  char file[3];
  int i, pid, n, fd;
  char fa[N];
  struct {
    ushort inum;
    char name[DIRSIZ];
  } de;

  printf("concreate test\n");
  file[0] = 'C';
  file[2] = '\0';
  for(i = 0; i < N; i++){
    file[1] = '0' + i;
    unlink(file);
    pid = fork();
    if(pid && (i % 3) == 1){
      link("C0", file);
    } else if(pid == 0 && (i % 5) == 1){
      link("C0", file);
    } else {
      fd = open(file, O_CREATE | O_RDWR);
      if(fd < 0){
        printf("concreate create %s failed\n", file);
        exit();
      }
      close(fd);
    }
    if(pid == 0)
      exit();
    else
      wait();
  }

  memset(fa, 0, sizeof(fa));
  fd = open(".", 0);
  n = 0;
  while(read(fd, &de, sizeof(de)) > 0){
    if(de.inum == 0)
      continue;
    if(de.name[0] == 'C' && de.name[2] == '\0'){
      i = de.name[1] - '0';
      if(i < 0 || i >= sizeof(fa)){
        printf("concreate weird file %s\n", de.name);
        exit();
      }
      if(fa[i]){
        printf("concreate duplicate file %s\n", de.name);
        exit();
      }
      fa[i] = 1;
      n++;
    }
  }
  close(fd);

  if(n != N){
    printf("concreate not enough files in directory listing\n");
    exit();
  }

  for(i = 0; i < N; i++){
    file[1] = '0' + i;
    pid = fork();
    if(pid < 0){
      printf("fork failed\n");
      exit();
    }
    if(((i % 3) == 0 && pid == 0) ||
       ((i % 3) == 1 && pid != 0)){
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
      close(open(file, 0));
    } else {
      unlink(file);
      unlink(file);
      unlink(file);
      unlink(file);
    }
    if(pid == 0)
      exit();
    else
      wait();
  }

  printf("concreate ok\n");
}

// another concurrent link/unlink/create test,
// to look for deadlocks.
void
linkunlink()
{
  int pid, i;

  printf("linkunlink test\n");

  unlink("x");
  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    exit();
  }

  unsigned int x = (pid ? 1 : 97);
  for(i = 0; i < 100; i++){
    x = x * 1103515245 + 12345;
    if((x % 3) == 0){
      close(open("x", O_RDWR | O_CREATE));
    } else if((x % 3) == 1){
      link("cat", "x");
    } else {
      unlink("x");
    }
  }

  if(pid)
    wait();
  else
    exit();

  printf("linkunlink ok\n");
}

// directory that uses indirect blocks
void
bigdir(void)
{
  enum { N = 500 };
  int i, fd;
  char name[10];

  printf("bigdir test\n");
  unlink("bd");

  fd = open("bd", O_CREATE);
  if(fd < 0){
    printf("bigdir create failed\n");
    exit();
  }
  close(fd);

  for(i = 0; i < N; i++){
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    if(link("bd", name) != 0){
      printf("bigdir link failed\n");
      exit();
    }
  }

  unlink("bd");
  for(i = 0; i < N; i++){
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    if(unlink(name) != 0){
      printf("bigdir unlink failed");
      exit();
    }
  }

  printf("bigdir ok\n");
}

void
subdir(void)
{
  int fd, cc;

  printf("subdir test\n");

  unlink("ff");
  if(mkdir("dd") != 0){
    printf("subdir mkdir dd failed\n");
    exit();
  }

  fd = open("dd/ff", O_CREATE | O_RDWR);
  if(fd < 0){
    printf("create dd/ff failed\n");
    exit();
  }
  write(fd, "ff", 2);
  close(fd);

  if(unlink("dd") >= 0){
    printf("unlink dd (non-empty dir) succeeded!\n");
    exit();
  }

  if(mkdir("/dd/dd") != 0){
    printf("subdir mkdir dd/dd failed\n");
    exit();
  }

  fd = open("dd/dd/ff", O_CREATE | O_RDWR);
  if(fd < 0){
    printf("create dd/dd/ff failed\n");
    exit();
  }
  write(fd, "FF", 2);
  close(fd);

  fd = open("dd/dd/../ff", 0);
  if(fd < 0){
    printf("open dd/dd/../ff failed\n");
    exit();
  }
  cc = read(fd, buf, sizeof(buf));
  if(cc != 2 || buf[0] != 'f'){
    printf("dd/dd/../ff wrong content\n");
    exit();
  }
  close(fd);

  if(link("dd/dd/ff", "dd/dd/ffff") != 0){
    printf("link dd/dd/ff dd/dd/ffff failed\n");
    exit();
  }

  if(unlink("dd/dd/ff") != 0){
    printf("unlink dd/dd/ff failed\n");
    exit();
  }
  if(open("dd/dd/ff", O_RDONLY) >= 0){
    printf("open (unlinked) dd/dd/ff succeeded\n");
    exit();
  }

  if(chdir("dd") != 0){
    printf("chdir dd failed\n");
    exit();
  }
  if(chdir("dd/../../dd") != 0){
    printf("chdir dd/../../dd failed\n");
    exit();
  }
  if(chdir("dd/../../../dd") != 0){
    printf("chdir dd/../../dd failed\n");
    exit();
  }
  if(chdir("./..") != 0){
    printf("chdir ./.. failed\n");
    exit();
  }

  fd = open("dd/dd/ffff", 0);
  if(fd < 0){
    printf("open dd/dd/ffff failed\n");
    exit();
  }
  if(read(fd, buf, sizeof(buf)) != 2){
    printf("read dd/dd/ffff wrong len\n");
    exit();
  }
  close(fd);

  if(open("dd/dd/ff", O_RDONLY) >= 0){
    printf("open (unlinked) dd/dd/ff succeeded!\n");
    exit();
  }

  if(open("dd/ff/ff", O_CREATE|O_RDWR) >= 0){
    printf("create dd/ff/ff succeeded!\n");
    exit();
  }
  if(open("dd/xx/ff", O_CREATE|O_RDWR) >= 0){
    printf("create dd/xx/ff succeeded!\n");
    exit();
  }
  if(open("dd", O_CREATE) >= 0){
    printf("create dd succeeded!\n");
    exit();
  }
  if(open("dd", O_RDWR) >= 0){
    printf("open dd rdwr succeeded!\n");
    exit();
  }
  if(open("dd", O_WRONLY) >= 0){
    printf("open dd wronly succeeded!\n");
    exit();
  }
  if(link("dd/ff/ff", "dd/dd/xx") == 0){
    printf("link dd/ff/ff dd/dd/xx succeeded!\n");
    exit();
  }
  if(link("dd/xx/ff", "dd/dd/xx") == 0){
    printf("link dd/xx/ff dd/dd/xx succeeded!\n");
    exit();
  }
  if(link("dd/ff", "dd/dd/ffff") == 0){
    printf("link dd/ff dd/dd/ffff succeeded!\n");
    exit();
  }
  if(mkdir("dd/ff/ff") == 0){
    printf("mkdir dd/ff/ff succeeded!\n");
    exit();
  }
  if(mkdir("dd/xx/ff") == 0){
    printf("mkdir dd/xx/ff succeeded!\n");
    exit();
  }
  if(mkdir("dd/dd/ffff") == 0){
    printf("mkdir dd/dd/ffff succeeded!\n");
    exit();
  }
  if(unlink("dd/xx/ff") == 0){
    printf("unlink dd/xx/ff succeeded!\n");
    exit();
  }
  if(unlink("dd/ff/ff") == 0){
    printf("unlink dd/ff/ff succeeded!\n");
    exit();
  }
  if(chdir("dd/ff") == 0){
    printf("chdir dd/ff succeeded!\n");
    exit();
  }
  if(chdir("dd/xx") == 0){
    printf("chdir dd/xx succeeded!\n");
    exit();
  }

  if(unlink("dd/dd/ffff") != 0){
    printf("unlink dd/dd/ff failed\n");
    exit();
  }
  if(unlink("dd/ff") != 0){
    printf("unlink dd/ff failed\n");
    exit();
  }
  if(unlink("dd") == 0){
    printf("unlink non-empty dd succeeded!\n");
    exit();
  }
  if(unlink("dd/dd") < 0){
    printf("unlink dd/dd failed\n");
    exit();
  }
  if(unlink("dd") < 0){
    printf("unlink dd failed\n");
    exit();
  }

  printf("subdir ok\n");
}

// test writes that are larger than the log.
void
bigwrite(void)
{
  int fd, sz;

  printf("bigwrite test\n");

  unlink("bigwrite");
  for(sz = 499; sz < (MAXOPBLOCKS+2)*BSIZE; sz += 471){
    fd = open("bigwrite", O_CREATE | O_RDWR);
    if(fd < 0){
      printf("cannot create bigwrite\n");
      exit();
    }
    int i;
    for(i = 0; i < 2; i++){
      int cc = write(fd, buf, sz);
      if(cc != sz){
        printf("write(%d) ret %d\n", sz, cc);
        exit();
      }
    }
    close(fd);
    unlink("bigwrite");
  }

  printf("bigwrite ok\n");
}

void
bigfile(void)
{
  enum { N = 20, SZ=600 };
  int fd, i, total, cc;

  printf("bigfile test\n");

  unlink("bigfile");
  fd = open("bigfile", O_CREATE | O_RDWR);
  if(fd < 0){
    printf("cannot create bigfile");
    exit();
  }
  for(i = 0; i < N; i++){
    memset(buf, i, SZ);
    if(write(fd, buf, SZ) != SZ){
      printf("write bigfile failed\n");
      exit();
    }
  }
  close(fd);

  fd = open("bigfile", 0);
  if(fd < 0){
    printf("cannot open bigfile\n");
    exit();
  }
  total = 0;
  for(i = 0; ; i++){
    cc = read(fd, buf, SZ/2);
    if(cc < 0){
      printf("read bigfile failed\n");
      exit();
    }
    if(cc == 0)
      break;
    if(cc != SZ/2){
      printf("short read bigfile\n");
      exit();
    }
    if(buf[0] != i/2 || buf[SZ/2-1] != i/2){
      printf("read bigfile wrong data\n");
      exit();
    }
    total += cc;
  }
  close(fd);
  if(total != N*SZ){
    printf("read bigfile wrong total\n");
    exit();
  }
  unlink("bigfile");

  printf("bigfile test ok\n");
}

void
fourteen(void)
{
  int fd;

  // DIRSIZ is 14.
  printf("fourteen test\n");

  if(mkdir("12345678901234") != 0){
    printf("mkdir 12345678901234 failed\n");
    exit();
  }
  if(mkdir("12345678901234/123456789012345") != 0){
    printf("mkdir 12345678901234/123456789012345 failed\n");
    exit();
  }
  fd = open("123456789012345/123456789012345/123456789012345", O_CREATE);
  if(fd < 0){
    printf("create 123456789012345/123456789012345/123456789012345 failed\n");
    exit();
  }
  close(fd);
  fd = open("12345678901234/12345678901234/12345678901234", 0);
  if(fd < 0){
    printf("open 12345678901234/12345678901234/12345678901234 failed\n");
    exit();
  }
  close(fd);

  if(mkdir("12345678901234/12345678901234") == 0){
    printf("mkdir 12345678901234/12345678901234 succeeded!\n");
    exit();
  }
  if(mkdir("123456789012345/12345678901234") == 0){
    printf("mkdir 12345678901234/123456789012345 succeeded!\n");
    exit();
  }

  printf("fourteen ok\n");
}

void
rmdot(void)
{
  printf("rmdot test\n");
  if(mkdir("dots") != 0){
    printf("mkdir dots failed\n");
    exit();
  }
  if(chdir("dots") != 0){
    printf("chdir dots failed\n");
    exit();
  }
  if(unlink(".") == 0){
    printf("rm . worked!\n");
    exit();
  }
  if(unlink("..") == 0){
    printf("rm .. worked!\n");
    exit();
  }
  if(chdir("/") != 0){
    printf("chdir / failed\n");
    exit();
  }
  if(unlink("dots/.") == 0){
    printf("unlink dots/. worked!\n");
    exit();
  }
  if(unlink("dots/..") == 0){
    printf("unlink dots/.. worked!\n");
    exit();
  }
  if(unlink("dots") != 0){
    printf("unlink dots failed!\n");
    exit();
  }
  printf("rmdot ok\n");
}

void
dirfile(void)
{
  int fd;

  printf("dir vs file\n");

  fd = open("dirfile", O_CREATE);
  if(fd < 0){
    printf("create dirfile failed\n");
    exit();
  }
  close(fd);
  if(chdir("dirfile") == 0){
    printf("chdir dirfile succeeded!\n");
    exit();
  }
  fd = open("dirfile/xx", 0);
  if(fd >= 0){
    printf("create dirfile/xx succeeded!\n");
    exit();
  }
  fd = open("dirfile/xx", O_CREATE);
  if(fd >= 0){
    printf("create dirfile/xx succeeded!\n");
    exit();
  }
  if(mkdir("dirfile/xx") == 0){
    printf("mkdir dirfile/xx succeeded!\n");
    exit();
  }
  if(unlink("dirfile/xx") == 0){
    printf("unlink dirfile/xx succeeded!\n");
    exit();
  }
  if(link("README", "dirfile/xx") == 0){
    printf("link to dirfile/xx succeeded!\n");
    exit();
  }
  if(unlink("dirfile") != 0){
    printf("unlink dirfile failed!\n");
    exit();
  }

  fd = open(".", O_RDWR);
  if(fd >= 0){
    printf("open . for writing succeeded!\n");
    exit();
  }
  fd = open(".", 0);
  if(write(fd, "x", 1) > 0){
    printf("write . succeeded!\n");
    exit();
  }
  close(fd);

  printf("dir vs file OK\n");
}

// test that iput() is called at the end of _namei()
void
iref(void)
{
  int i, fd;

  printf("empty file name\n");

  for(i = 0; i < NINODE + 1; i++){
    if(mkdir("irefd") != 0){
      printf("mkdir irefd failed\n");
      exit();
    }
    if(chdir("irefd") != 0){
      printf("chdir irefd failed\n");
      exit();
    }

    mkdir("");
    link("README", "");
    fd = open("", O_CREATE);
    if(fd >= 0)
      close(fd);
    fd = open("xx", O_CREATE);
    if(fd >= 0)
      close(fd);
    unlink("xx");
  }

  chdir("/");
  printf("empty file name OK\n");
}

// test that fork fails gracefully
// the forktest binary also does this, but it runs out of proc entries first.
// inside the bigger usertests binary, we run out of memory first.
void
forktest(void)
{
  enum{ N = 1000 };
  int n, pid;

  printf("fork test\n");

  for(n=0; n<N; n++){
    pid = fork();
    if(pid < 0)
      break;
    if(pid == 0)
      exit();
  }

  if (n == 0) {
    printf("no fork at all!\n");
    exit();
  }

  if(n == N){
    printf("fork claimed to work 1000 times!\n");
    exit();
  }

  for(; n > 0; n--){
    if(wait() < 0){
      printf("wait stopped early\n");
      exit();
    }
  }

  if(wait() != -1){
    printf("wait got too many\n");
    exit();
  }

  printf("fork test OK\n");
}

void
sbrktest(void)
{
  enum { BIG=100*1024*1024, TOOMUCH=1024*1024*1024};
  int i, fds[2], pids[10], pid, ppid;
  char *c, *oldbrk, scratch, *a, *b, *lastaddr, *p;
  uint64 amt;
  int fd;
  int n;

  printf("sbrk test\n");
  oldbrk = sbrk(0);

  // does sbrk() return the expected failure value?
  a = sbrk(TOOMUCH);
  if(a != (char*)0xffffffffffffffffL){
    printf("sbrk(<toomuch>) returned %p\n", a);
    exit();
  }

  // can one sbrk() less than a page?
  a = sbrk(0);
  for(i = 0; i < 5000; i++){
    b = sbrk(1);
    if(b != a){
      printf("sbrk test failed %d %x %x\n", i, a, b);
      exit();
    }
    *b = 1;
    a = b + 1;
  }
  pid = fork();
  if(pid < 0){
    printf("sbrk test fork failed\n");
    exit();
  }
  c = sbrk(1);
  c = sbrk(1);
  if(c != a + 1){
    printf("sbrk test failed post-fork\n");
    exit();
  }
  if(pid == 0)
    exit();
  wait();

  // can one grow address space to something big?
  a = sbrk(0);
  amt = BIG - (uint64)a;
  p = sbrk(amt);
  if (p != a) {
    printf("sbrk test failed to grow big address space; enough phys mem?\n");
    exit();
  }
  lastaddr = (char*) (BIG-1);
  *lastaddr = 99;

  // can one de-allocate?
  a = sbrk(0);
  c = sbrk(-PGSIZE);
  if(c == (char*)0xffffffffffffffffL){
    printf("sbrk could not deallocate\n");
    exit();
  }
  c = sbrk(0);
  if(c != a - PGSIZE){
    printf("sbrk deallocation produced wrong address, a %x c %x\n", a, c);
    exit();
  }

  // can one re-allocate that page?
  a = sbrk(0);
  c = sbrk(PGSIZE);
  if(c != a || sbrk(0) != a + PGSIZE){
    printf("sbrk re-allocation failed, a %x c %x\n", a, c);
    exit();
  }
  if(*lastaddr == 99){
    // should be zero
    printf("sbrk de-allocation didn't really deallocate\n");
    exit();
  }

  a = sbrk(0);
  c = sbrk(-(sbrk(0) - oldbrk));
  if(c != a){
    printf("sbrk downsize failed, a %x c %x\n", a, c);
    exit();
  }

  // can we read the kernel's memory?
  for(a = (char*)(KERNBASE); a < (char*) (KERNBASE+2000000); a += 50000){
    ppid = getpid();
    pid = fork();
    if(pid < 0){
      printf("fork failed\n");
      exit();
    }
    if(pid == 0){
      printf("oops could read %x = %x\n", a, *a);
      kill(ppid);
      exit();
    }
    wait();
  }
    
  // if we run the system out of memory, does it clean up the last
  // failed allocation?
  if(pipe(fds) != 0){
    printf("pipe() failed\n");
    exit();
  }
  for(i = 0; i < sizeof(pids)/sizeof(pids[0]); i++){
    if((pids[i] = fork()) == 0){
      // allocate a lot of memory
      sbrk(BIG - (uint64)sbrk(0));
      write(fds[1], "x", 1);
      // sit around until killed
      for(;;) sleep(1000);
    }
    if(pids[i] != -1)
      read(fds[0], &scratch, 1);
  }

  // if those failed allocations freed up the pages they did allocate,
  // we'll be able to allocate here
  c = sbrk(PGSIZE);
  for(i = 0; i < sizeof(pids)/sizeof(pids[0]); i++){
    if(pids[i] == -1)
      continue;
    kill(pids[i]);
    wait();
  }
  if(c == (char*)0xffffffffffffffffL){
    printf("failed sbrk leaked memory\n");
    exit();
  }

  // test running fork with the above allocated page 
  ppid = getpid();
  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    exit();
  }

  // test out of memory during sbrk
  if(pid == 0){
    // allocate a lot of memory
    a = sbrk(0);
    sbrk(10*BIG);
    int n = 0;
    for (i = 0; i < 10*BIG; i += PGSIZE) {
      n += *(a+i);
    }
    printf("allocate a lot of memory succeeded %d\n", n);
    kill(ppid);
    exit();
  }
  wait();

  // test reads from allocated memory
  a = sbrk(PGSIZE);
  fd = open("sbrk", O_CREATE|O_WRONLY);
  unlink("sbrk");
  if(fd < 0)  {
    printf("open sbrk failed\n");
    exit();
  }
  if ((n = write(fd, a, 10)) < 0) {
    printf("write sbrk failed\n");
    exit();
  }
  close(fd);

  // test writes to allocated memory
  a = sbrk(PGSIZE);
  if(pipe((int *) a) != 0){
    printf("pipe() failed\n");
    exit();
  } 

  if(sbrk(0) > oldbrk)
    sbrk(-(sbrk(0) - oldbrk));

  printf("sbrk test OK\n");
}

void
validatetest(void)
{
  int hi;
  uint64 p;

  printf("validate test\n");
  hi = 1100*1024;

  for(p = 0; p <= (uint)hi; p += PGSIZE){
    // try to crash the kernel by passing in a bad string pointer
    if(link("nosuchfile", (char*)p) != -1){
      printf("link should not succeed\n");
      exit();
    }
  }

  printf("validate ok\n");
}

// does unintialized data start out zero?
char uninit[10000];
void
bsstest(void)
{
  int i;

  printf("bss test\n");
  for(i = 0; i < sizeof(uninit); i++){
    if(uninit[i] != '\0'){
      printf("bss test failed\n");
      exit();
    }
  }
  printf("bss test ok\n");
}

// does exec return an error if the arguments
// are larger than a page? or does it write
// below the stack and wreck the instructions/data?
void
bigargtest(void)
{
  int pid, fd;

  unlink("bigarg-ok");
  pid = fork();
  if(pid == 0){
    static char *args[MAXARG];
    int i;
    for(i = 0; i < MAXARG-1; i++)
      args[i] = "bigargs test: failed\n                                                                                                                                                                                                       ";
    args[MAXARG-1] = 0;
    printf("bigarg test\n");
    exec("echo", args);
    printf("bigarg test ok\n");
    fd = open("bigarg-ok", O_CREATE);
    close(fd);
    exit();
  } else if(pid < 0){
    printf("bigargtest: fork failed\n");
    exit();
  }
  wait();
  fd = open("bigarg-ok", 0);
  if(fd < 0){
    printf("bigarg test failed!\n");
    exit();
  }
  close(fd);
  unlink("bigarg-ok");
}

// what happens when the file system runs out of blocks?
// answer: balloc panics, so this test is not useful.
void
fsfull()
{
  int nfiles;
  int fsblocks = 0;

  printf("fsfull test\n");

  for(nfiles = 0; ; nfiles++){
    char name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    printf("writing %s\n", name);
    int fd = open(name, O_CREATE|O_RDWR);
    if(fd < 0){
      printf("open %s failed\n", name);
      break;
    }
    int total = 0;
    while(1){
      int cc = write(fd, buf, BSIZE);
      if(cc < BSIZE)
        break;
      total += cc;
      fsblocks++;
    }
    printf("wrote %d bytes\n", total);
    close(fd);
    if(total == 0)
      break;
  }

  while(nfiles >= 0){
    char name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    unlink(name);
    nfiles--;
  }

  printf("fsfull test finished\n");
}

void argptest()
{
  int fd;
  fd = open("init", O_RDONLY);
  if (fd < 0) {
    fprintf(2, "open failed\n");
    exit();
  }
  read(fd, sbrk(0) - 1, -1);
  close(fd);
  printf("arg test passed\n");
}

unsigned long randstate = 1;
unsigned int
rand()
{
  randstate = randstate * 1664525 + 1013904223;
  return randstate;
}

// check that there's an invalid page beneath
// the user stack, to catch stack overflow.
void
stacktest()
{
  int pid;
  int ppid = getpid();
  
  printf("stack guard test\n");
  pid = fork();
  if(pid == 0) {
    char *sp = (char *) r_sp();
    sp -= PGSIZE;
    // the *sp should cause a trap.
    printf("stacktest: read below stack %p\n", *sp);
    printf("stacktest: test FAILED\n");
    kill(ppid);
    exit();
  } else if(pid < 0){
    printf("fork failed\n");
    exit();
  }
  wait();
  printf("stack guard test ok\n");
}

int
main(int argc, char *argv[])
{
  printf("usertests starting\n");

  if(open("usertests.ran", 0) >= 0){
    printf("already ran user tests -- rebuild fs.img\n");
    exit();
  }
  close(open("usertests.ran", O_CREATE));

  reparent();
  twochildren();
  forkfork();
  forkforkfork();
  
  argptest();
  createdelete();
  linkunlink();
  concreate();
  fourfiles();
  sharedfd();

  bigargtest();
  bigwrite();
  bigargtest();
  bsstest();
  sbrktest();
  validatetest();
  stacktest();
  
  opentest();
  writetest();
  writetest1();
  createtest();

  openiputtest();
  exitiputtest();
  iputtest();

  mem();
  pipe1();
  preempt();
  exitwait();

  rmdot();
  fourteen();
  bigfile();
  subdir();
  linktest();
  unlinkread();
  dirfile();
  iref();
  forktest();
  bigdir(); // slow

  exectest();

  exit();
}
