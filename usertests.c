#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

char buf[8192];
char name[3];
char *echoargv[] = { "echo", "ALL", "TESTS", "PASSED", 0 };
char *echoargv1[] = { "echo", "b", "c", 0 };
int stdout = 1;

// does chdir() call iput(p->cwd) in a transaction?
int
iputtest(void)
{
  printf(stdout, "iput test\n");

  if(mkdir("iputdir") < 0){
    printf(stdout, "mkdir failed\n");
    return 1;
  }
  if(chdir("iputdir") < 0){
    printf(stdout, "chdir iputdir failed\n");
    return 1;
  }
  if(unlink("../iputdir") < 0){
    printf(stdout, "unlink ../iputdir failed\n");
    return 1;
  }
  if(chdir("/") < 0){
    printf(stdout, "chdir / failed\n");
    return 1;
  }
  printf(stdout, "iput test ok\n");
  return 0;
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
int
openiputtest(void)
{
  int pid;

  printf(stdout, "openiput test\n");
  if(mkdir("oidir") < 0){
    printf(stdout, "mkdir oidir failed\n");
    return 1;
  }
  pid = fork();
  if(pid < 0){
    printf(stdout, "fork failed\n");
    return 1;
  }
  if(pid == 0){
    int fd = open("oidir", O_RDWR);
    if(fd >= 0){
      printf(stdout, "open directory for write succeeded\n");
      exit();
    }
    exit();
  }
  sleep(1);
  if(unlink("oidir") != 0){
    printf(stdout, "unlink failed\n");
    return 1;
  }
  wait();
  printf(stdout, "openiput test ok\n");
  return 0;
}

// simple file system tests

int
opentest(void)
{
  int fd;

  printf(stdout, "open test\n");
  fd = open("echo", 0);
  if(fd < 0){
    printf(stdout, "open echo failed!\n");
    return 1;
  }
  close(fd);
  fd = open("doesnotexist", 0);
  if(fd >= 0){
    printf(stdout, "open doesnotexist succeeded!\n");
    return 1;
  }
  printf(stdout, "open test ok\n");
  return 0;
}

int
writetest(void)
{
  int fd;
  int i;

  printf(stdout, "small file test\n");
  fd = open("small", O_CREATE|O_RDWR);
  if(fd >= 0){
    printf(stdout, "creat small succeeded; ok\n");
  } else {
    printf(stdout, "error: creat small failed!\n");
    return 1;
  }
  for(i = 0; i < 100; i++){
    if(write(fd, "aaaaaaaaaa", 10) != 10){
      printf(stdout, "error: write aa %d new file failed\n", i);
      return 1;
    }
    if(write(fd, "bbbbbbbbbb", 10) != 10){
      printf(stdout, "error: write bb %d new file failed\n", i);
      return 1;
    }
  }
  printf(stdout, "writes ok\n");
  close(fd);
  fd = open("small", O_RDONLY);
  if(fd >= 0){
    printf(stdout, "open small succeeded ok\n");
  } else {
    printf(stdout, "error: open small failed!\n");
    return 1;
  }
  i = read(fd, buf, 2000);
  if(i == 2000){
    printf(stdout, "read succeeded ok\n");
  } else {
    printf(stdout, "read failed\n");
    return 1;
  }
  close(fd);

  if(unlink("small") < 0){
    printf(stdout, "unlink small failed\n");
    return 1;
  }
  printf(stdout, "small file test ok\n");
  return 0;
}

int
writetest1(void)
{
  int i, fd, n;

  printf(stdout, "big files test\n");

  fd = open("big", O_CREATE|O_RDWR);
  if(fd < 0){
    printf(stdout, "error: creat big failed!\n");
    return 1;
  }

  for(i = 0; i < MAXFILE; i++){
    ((int*)buf)[0] = i;
    if(write(fd, buf, 512) != 512){
      printf(stdout, "error: write big file failed\n", i);
      return 1;
    }
  }

  close(fd);

  fd = open("big", O_RDONLY);
  if(fd < 0){
    printf(stdout, "error: open big failed!\n");
    return 1;
  }

  n = 0;
  for(;;){
    i = read(fd, buf, 512);
    if(i == 0){
      if(n == MAXFILE - 1){
        printf(stdout, "read only %d blocks from big", n);
        return 1;
      }
      break;
    } else if(i != 512){
      printf(stdout, "read failed %d\n", i);
      return 1;
    }
    if(((int*)buf)[0] != n){
      printf(stdout, "read content of block %d is %d\n",
             n, ((int*)buf)[0]);
      return 1;
    }
    n++;
  }
  close(fd);
  if(unlink("big") < 0){
    printf(stdout, "unlink big failed\n");
    return 1;
  }
  printf(stdout, "big files ok\n");
  return 0;
}

int
createtest(void)
{
  int i, fd;

  printf(stdout, "many creates, followed by unlink test\n");

  name[0] = 'a';
  name[2] = '\0';
  for(i = 0; i < 52; i++){
    name[1] = '0' + i;
    fd = open(name, O_CREATE|O_RDWR);
    close(fd);
  }
  name[0] = 'a';
  name[2] = '\0';
  for(i = 0; i < 52; i++){
    name[1] = '0' + i;
    unlink(name);
  }
  printf(stdout, "many creates, followed by unlink; ok\n");
  return 0;
}

int
exectest(void)
{
  printf(stdout, "exec test\n");
  if(exec("echo", echoargv) < 0){
    printf(stdout, "exec echo failed\n");
    return 1;
  }
  printf(stdout, "exec test ok\n");
  return 0;
}

// simple fork and pipe read/write

int
pipe1(void)
{
  int fds[2], pid;
  int seq, i, n, cc, total;

  if(pipe(fds) != 0){
    printf(1, "pipe() failed\n");
    return 1;
  }
  pid = fork();
  seq = 0;
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
  } else if(pid > 0){
    close(fds[1]);
    total = 0;
    cc = 1;
    while((n = read(fds[0], buf, cc)) > 0){
      for(i = 0; i < n; i++){
        if((buf[i] & 0xff) != (seq++ & 0xff)){
          printf(1, "pipe1 oops 2\n");
          return 1;
        }
      }
      total += n;
      cc = cc * 2;
      if(cc > sizeof(buf))
        cc = sizeof(buf);
    }
    if(total != 5 * 1033){
      printf(1, "pipe1 oops 3 total %d\n", total);
      return 1;
    }
    close(fds[0]);
    wait();
  } else {
    printf(1, "fork() failed\n");
    return 1;
  }
  printf(1, "pipe1 ok\n");
  return 0;
}

// meant to be run w/ at most two CPUs
int
preempt(void)
{
  int pid1, pid2, pid3;
  int pfds[2];

  printf(1, "preempt: ");
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
    return 1;
  }
  close(pfds[0]);
  printf(1, "kill... ");
  kill(pid1);
  kill(pid2);
  kill(pid3);
  printf(1, "wait... ");
  wait();
  wait();
  wait();
  printf(1, "preempt ok\n");
  return 0;
}


int
mem(void)
{
  void *m1, *m2;
  int pid, ppid;

  printf(1, "mem test\n");
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
      printf(1, "couldn't allocate mem?!!\n");
      kill(ppid);
      exit();
    }
    free(m1);
    printf(1, "mem ok\n");
    exit();
  } else {
    wait();
    return 0;
  }
}

// More file system tests

// two processes write to the same file descriptor
// is the offset shared? does inode locking work?
int
sharedfd(void)
{
  int fd, pid, i, n, nc, np;
  char buf[10];

  printf(1, "sharedfd test\n");

  unlink("sharedfd");
  fd = open("sharedfd", O_CREATE|O_RDWR);
  if(fd < 0){
    printf(1, "fstests: cannot open sharedfd for writing");
    return 1;
  }
  pid = fork();
  memset(buf, pid==0?'c':'p', sizeof(buf));
  for(i = 0; i < 1000; i++){
    if(write(fd, buf, sizeof(buf)) != sizeof(buf)){
      printf(1, "fstests: write sharedfd failed\n");
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
    printf(1, "fstests: cannot open sharedfd for reading\n");
    return 1;
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
  if(nc == 10000 && np == 10000){
    printf(1, "sharedfd ok\n");
    return 0;
  } else {
    printf(1, "sharedfd oops %d %d\n", nc, np);
    return 1;
  }
}

// four processes write different files at the same
// time, to test block allocation.
int
fourfiles(void)
{
  int fd, pid, i, j, n, total, pi;
  char *names[] = { "f0", "f1", "f2", "f3" };
  char *fname;

  printf(1, "fourfiles test\n");

  for(pi = 0; pi < 4; pi++){
    fname = names[pi];
    unlink(fname);

    pid = fork();
    if(pid < 0){
      printf(1, "fork failed\n");
      exit();
    }

    if(pid == 0){
      fd = open(fname, O_CREATE | O_RDWR);
      if(fd < 0){
        printf(1, "create failed\n");
        exit();
      }
      
      memset(buf, '0'+pi, 512);
      for(i = 0; i < 12; i++){
        if((n = write(fd, buf, 500)) != 500){
          printf(1, "write failed %d\n", n);
          exit();
        }
      }
      exit();
    }
  }

  for(pi = 0; pi < 4; pi++){
    wait();
  }

  for(i = 0; i < 2; i++){
    fname = names[i];
    fd = open(fname, 0);
    total = 0;
    while((n = read(fd, buf, sizeof(buf))) > 0){
      for(j = 0; j < n; j++){
        if(buf[j] != '0'+i){
          printf(1, "wrong char\n");
          return 1;
        }
      }
      total += n;
    }
    close(fd);
    if(total != 12*500){
      printf(1, "wrong length %d\n", total);
      return 1;
    }
    unlink(fname);
  }

  for(i = 0; i < 4; i++){
    fname = names[i];
    unlink(fname);
  }

  printf(1, "fourfiles ok\n");
  return 0;
}

// four processes create and delete different files in same directory
int
createdelete(void)
{
  enum { N = 20 };
  int pid, i, fd, pi;
  char name[32];

  printf(1, "createdelete test\n");

  for(pi = 0; pi < 4; pi++){
    pid = fork();
    if(pid < 0){
      printf(1, "fork failed\n");
      return 1;
    }

    if(pid == 0){
      name[0] = 'p' + pi;
      name[2] = '\0';
      for(i = 0; i < N; i++){
        name[1] = '0' + i;
        fd = open(name, O_CREATE | O_RDWR);
        if(fd < 0){
          printf(1, "create failed\n");
          exit();
        }
        close(fd);
        if(i > 0 && (i % 2 ) == 0){
          name[1] = '0' + (i / 2);
          if(unlink(name) < 0){
            printf(1, "unlink failed\n");
            exit();
          }
        }
      }
      exit();
    }
  }

  for(pi = 0; pi < 4; pi++){
    wait();
  }

  name[0] = name[1] = name[2] = 0;
  for(i = 0; i < N; i++){
    for(pi = 0; pi < 4; pi++){
      name[0] = 'p' + pi;
      name[1] = '0' + i;
      fd = open(name, 0);
      if((i == 0 || i >= N/2) && fd < 0){
        printf(1, "oops createdelete %s didn't exist\n", name);
        return 1;
      } else if((i >= 1 && i < N/2) && fd >= 0){
        printf(1, "oops createdelete %s did exist\n", name);
        return 1;
      }
      if(fd >= 0)
        close(fd);
    }
  }

  for(i = 0; i < N; i++){
    for(pi = 0; pi < 4; pi++){
      name[0] = 'p' + i;
      name[1] = '0' + i;
      unlink(name);
    }
  }

  printf(1, "createdelete ok\n");
  return 0;
}

// can I unlink a file and still read it?
int
unlinkread(void)
{
  int fd, fd1;

  printf(1, "unlinkread test\n");
  fd = open("unlinkread", O_CREATE | O_RDWR);
  if(fd < 0){
    printf(1, "create unlinkread failed\n");
    return 1;
  }
  write(fd, "hello", 5);
  close(fd);

  fd = open("unlinkread", O_RDWR);
  if(fd < 0){
    printf(1, "open unlinkread failed\n");
    return 1;
  }
  if(unlink("unlinkread") != 0){
    printf(1, "unlink unlinkread failed\n");
    return 1;
  }

  fd1 = open("unlinkread", O_CREATE | O_RDWR);
  write(fd1, "yyy", 3);
  close(fd1);

  if(read(fd, buf, sizeof(buf)) != 5){
    printf(1, "unlinkread read failed");
    return 1;
  }
  if(buf[0] != 'h'){
    printf(1, "unlinkread wrong data\n");
    return 1;
  }
  if(write(fd, buf, 10) != 10){
    printf(1, "unlinkread write failed\n");
    return 1;
  }
  close(fd);
  unlink("unlinkread");
  printf(1, "unlinkread ok\n");
  return 0;
}

int
linktest(void)
{
  int fd;

  printf(1, "linktest\n");

  unlink("lf1");
  unlink("lf2");

  fd = open("lf1", O_CREATE|O_RDWR);
  if(fd < 0){
    printf(1, "create lf1 failed\n");
    return 1;
  }
  if(write(fd, "hello", 5) != 5){
    printf(1, "write lf1 failed\n");
    return 1;
  }
  close(fd);

  if(link("lf1", "lf2") < 0){
    printf(1, "link lf1 lf2 failed\n");
    return 1;
  }
  unlink("lf1");

  if(open("lf1", 0) >= 0){
    printf(1, "unlinked lf1 but it is still there!\n");
    return 1;
  }

  fd = open("lf2", 0);
  if(fd < 0){
    printf(1, "open lf2 failed\n");
    return 1;
  }
  if(read(fd, buf, sizeof(buf)) != 5){
    printf(1, "read lf2 failed\n");
    return 1;
  }
  close(fd);

  if(link("lf2", "lf2") >= 0){
    printf(1, "link lf2 lf2 succeeded! oops\n");
    return 1;
  }

  unlink("lf2");
  if(link("lf2", "lf1") >= 0){
    printf(1, "link non-existant succeeded! oops\n");
    return 1;
  }

  if(link(".", "lf1") >= 0){
    printf(1, "link . lf1 succeeded! oops\n");
    return 1;
  }

  fd = open("lf1", O_CREATE|O_RDWR);
  if(fd < 0){
    printf(1, "create lf1 failed\n");
    return 1;
  }

  if(link("lf1", "lf2") < 0){
    printf(1, "link failed\n");
    return 1;
  }
  if(link("lf2", "lf3") < 0){
    printf(1, "double link failed\n");
    return 1;
  }
  unlink("lf1");
  unlink("lf2");
  unlink("lf3");

  printf(1, "linktest ok\n");
  return 0;
}

// test concurrent create/link/unlink of the same file
int
concreate(void)
{
  char file[3];
  int i, pid, n, fd;
  char fa[40];
  struct {
    ushort inum;
    char name[14];
  } de;

  printf(1, "concreate test\n");
  file[0] = 'C';
  file[2] = '\0';
  for(i = 0; i < 40; i++){
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
        printf(1, "concreate create %s failed\n", file);
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
        printf(1, "concreate weird file %s\n", de.name);
        return 1;
      }
      if(fa[i]){
        printf(1, "concreate duplicate file %s\n", de.name);
        return 1;
      }
      fa[i] = 1;
      n++;
    }
  }
  close(fd);

  if(n != 40){
    printf(1, "concreate not enough files in directory listing\n");
    return 1;
  }

  for(i = 0; i < 40; i++){
    file[1] = '0' + i;
    pid = fork();
    if(pid < 0){
      printf(1, "fork failed\n");
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

  printf(1, "concreate ok\n");
  return 0;
}

// another concurrent link/unlink/create test,
// to look for deadlocks.
int
linkunlink()
{
  int pid, i;

  printf(1, "linkunlink test\n");

  unlink("x");
  pid = fork();
  if(pid < 0){
    printf(1, "fork failed\n");
    return 1;
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

  printf(1, "linkunlink ok\n");
  return 0;
}

// directory that uses indirect blocks
int
bigdir(void)
{
  int i, fd;
  char name[10];

  printf(1, "bigdir test\n");
  unlink("bd");

  fd = open("bd", O_CREATE);
  if(fd < 0){
    printf(1, "bigdir create failed\n");
    return 1;
  }
  close(fd);

  for(i = 0; i < 500; i++){
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    if(link("bd", name) != 0){
      printf(1, "bigdir link failed\n");
      return 1;
    }
  }

  unlink("bd");
  for(i = 0; i < 500; i++){
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    if(unlink(name) != 0){
      printf(1, "bigdir unlink failed");
      return 1;
    }
  }

  printf(1, "bigdir ok\n");
  return 0;
}

int
subdir(void)
{
  int fd, cc;

  printf(1, "subdir test\n");

  unlink("ff");
  if(mkdir("dd") != 0){
    printf(1, "subdir mkdir dd failed\n");
    return 1;
  }

  fd = open("dd/ff", O_CREATE | O_RDWR);
  if(fd < 0){
    printf(1, "create dd/ff failed\n");
    return 1;
  }
  write(fd, "ff", 2);
  close(fd);
  
  if(unlink("dd") >= 0){
    printf(1, "unlink dd (non-empty dir) succeeded!\n");
    return 1;
  }

  if(mkdir("/dd/dd") != 0){
    printf(1, "subdir mkdir dd/dd failed\n");
    return 1;
  }

  fd = open("dd/dd/ff", O_CREATE | O_RDWR);
  if(fd < 0){
    printf(1, "create dd/dd/ff failed\n");
    return 1;
  }
  write(fd, "FF", 2);
  close(fd);

  fd = open("dd/dd/../ff", 0);
  if(fd < 0){
    printf(1, "open dd/dd/../ff failed\n");
    return 1;
  }
  cc = read(fd, buf, sizeof(buf));
  if(cc != 2 || buf[0] != 'f'){
    printf(1, "dd/dd/../ff wrong content\n");
    return 1;
  }
  close(fd);

  if(link("dd/dd/ff", "dd/dd/ffff") != 0){
    printf(1, "link dd/dd/ff dd/dd/ffff failed\n");
    return 1;
  }

  if(unlink("dd/dd/ff") != 0){
    printf(1, "unlink dd/dd/ff failed\n");
    return 1;
  }
  if(open("dd/dd/ff", O_RDONLY) >= 0){
    printf(1, "open (unlinked) dd/dd/ff succeeded\n");
    return 1;
  }

  if(chdir("dd") != 0){
    printf(1, "chdir dd failed\n");
    return 1;
  }
  if(chdir("dd/../../dd") != 0){
    printf(1, "chdir dd/../../dd failed\n");
    return 1;
  }
  if(chdir("dd/../../../dd") != 0){
    printf(1, "chdir dd/../../dd failed\n");
    return 1;
  }
  if(chdir("./..") != 0){
    printf(1, "chdir ./.. failed\n");
    return 1;
  }

  fd = open("dd/dd/ffff", 0);
  if(fd < 0){
    printf(1, "open dd/dd/ffff failed\n");
    return 1;
  }
  if(read(fd, buf, sizeof(buf)) != 2){
    printf(1, "read dd/dd/ffff wrong len\n");
    return 1;
  }
  close(fd);

  if(open("dd/dd/ff", O_RDONLY) >= 0){
    printf(1, "open (unlinked) dd/dd/ff succeeded!\n");
    return 1;
  }

  if(open("dd/ff/ff", O_CREATE|O_RDWR) >= 0){
    printf(1, "create dd/ff/ff succeeded!\n");
    return 1;
  }
  if(open("dd/xx/ff", O_CREATE|O_RDWR) >= 0){
    printf(1, "create dd/xx/ff succeeded!\n");
    return 1;
  }
  if(open("dd", O_CREATE) >= 0){
    printf(1, "create dd succeeded!\n");
    return 1;
  }
  if(open("dd", O_RDWR) >= 0){
    printf(1, "open dd rdwr succeeded!\n");
    return 1;
  }
  if(open("dd", O_WRONLY) >= 0){
    printf(1, "open dd wronly succeeded!\n");
    return 1;
  }
  if(link("dd/ff/ff", "dd/dd/xx") == 0){
    printf(1, "link dd/ff/ff dd/dd/xx succeeded!\n");
    return 1;
  }
  if(link("dd/xx/ff", "dd/dd/xx") == 0){
    printf(1, "link dd/xx/ff dd/dd/xx succeeded!\n");
    return 1;
  }
  if(link("dd/ff", "dd/dd/ffff") == 0){
    printf(1, "link dd/ff dd/dd/ffff succeeded!\n");
    return 1;
  }
  if(mkdir("dd/ff/ff") == 0){
    printf(1, "mkdir dd/ff/ff succeeded!\n");
    return 1;
  }
  if(mkdir("dd/xx/ff") == 0){
    printf(1, "mkdir dd/xx/ff succeeded!\n");
    return 1;
  }
  if(mkdir("dd/dd/ffff") == 0){
    printf(1, "mkdir dd/dd/ffff succeeded!\n");
    return 1;
  }
  if(unlink("dd/xx/ff") == 0){
    printf(1, "unlink dd/xx/ff succeeded!\n");
    return 1;
  }
  if(unlink("dd/ff/ff") == 0){
    printf(1, "unlink dd/ff/ff succeeded!\n");
    return 1;
  }
  if(chdir("dd/ff") == 0){
    printf(1, "chdir dd/ff succeeded!\n");
    return 1;
  }
  if(chdir("dd/xx") == 0){
    printf(1, "chdir dd/xx succeeded!\n");
    return 1;
  }

  if(unlink("dd/dd/ffff") != 0){
    printf(1, "unlink dd/dd/ff failed\n");
    return 1;
  }
  if(unlink("dd/ff") != 0){
    printf(1, "unlink dd/ff failed\n");
    return 1;
  }
  if(unlink("dd") == 0){
    printf(1, "unlink non-empty dd succeeded!\n");
    return 1;
  }
  if(unlink("dd/dd") < 0){
    printf(1, "unlink dd/dd failed\n");
    return 1;
  }
  if(unlink("dd") < 0){
    printf(1, "unlink dd failed\n");
    return 1;
  }

  printf(1, "subdir ok\n");
  return 0;
}

// test writes that are larger than the log.
int
bigwrite(void)
{
  int fd, sz;

  printf(1, "bigwrite test\n");

  unlink("bigwrite");
  for(sz = 499; sz < 12*512; sz += 471){
    fd = open("bigwrite", O_CREATE | O_RDWR);
    if(fd < 0){
      printf(1, "cannot create bigwrite\n");
      return 1;
    }
    int i;
    for(i = 0; i < 2; i++){
      int cc = write(fd, buf, sz);
      if(cc != sz){
        printf(1, "write(%d) ret %d\n", sz, cc);
        return 1;
      }
    }
    close(fd);
    unlink("bigwrite");
  }

  printf(1, "bigwrite ok\n");
  return 0;
}

int
bigfile(void)
{
  int fd, i, total, cc;

  printf(1, "bigfile test\n");

  unlink("bigfile");
  fd = open("bigfile", O_CREATE | O_RDWR);
  if(fd < 0){
    printf(1, "cannot create bigfile");
    return 1;
  }
  for(i = 0; i < 20; i++){
    memset(buf, i, 600);
    if(write(fd, buf, 600) != 600){
      printf(1, "write bigfile failed\n");
      return 1;
    }
  }
  close(fd);

  fd = open("bigfile", 0);
  if(fd < 0){
    printf(1, "cannot open bigfile\n");
    return 1;
  }
  total = 0;
  for(i = 0; ; i++){
    cc = read(fd, buf, 300);
    if(cc < 0){
      printf(1, "read bigfile failed\n");
      return 1;
    }
    if(cc == 0)
      break;
    if(cc != 300){
      printf(1, "short read bigfile\n");
      return 1;
    }
    if(buf[0] != i/2 || buf[299] != i/2){
      printf(1, "read bigfile wrong data\n");
      return 1;
    }
    total += cc;
  }
  close(fd);
  if(total != 20*600){
    printf(1, "read bigfile wrong total\n");
    return 1;
  }
  unlink("bigfile");

  printf(1, "bigfile test ok\n");
  return 0;
}

int
fourteen(void)
{
  int fd;

  // DIRSIZ is 14.
  printf(1, "fourteen test\n");

  if(mkdir("12345678901234") != 0){
    printf(1, "mkdir 12345678901234 failed\n");
    return 1;
  }
  if(mkdir("12345678901234/123456789012345") != 0){
    printf(1, "mkdir 12345678901234/123456789012345 failed\n");
    return 1;
  }
  fd = open("123456789012345/123456789012345/123456789012345", O_CREATE);
  if(fd < 0){
    printf(1, "create 123456789012345/123456789012345/123456789012345 failed\n");
    return 1;
  }
  close(fd);
  fd = open("12345678901234/12345678901234/12345678901234", 0);
  if(fd < 0){
    printf(1, "open 12345678901234/12345678901234/12345678901234 failed\n");
    return 1;
  }
  close(fd);

  if(mkdir("12345678901234/12345678901234") == 0){
    printf(1, "mkdir 12345678901234/12345678901234 succeeded!\n");
    return 1;
  }
  if(mkdir("123456789012345/12345678901234") == 0){
    printf(1, "mkdir 12345678901234/123456789012345 succeeded!\n");
    return 1;
  }

  printf(1, "fourteen ok\n");
  return 0;
}

int
rmdot(void)
{
  printf(1, "rmdot test\n");
  if(mkdir("dots") != 0){
    printf(1, "mkdir dots failed\n");
    return 1;
  }
  if(chdir("dots") != 0){
    printf(1, "chdir dots failed\n");
    return 1;
  }
  if(unlink(".") == 0){
    printf(1, "rm . worked!\n");
    return 1;
  }
  if(unlink("..") == 0){
    printf(1, "rm .. worked!\n");
    return 1;
  }
  if(chdir("/") != 0){
    printf(1, "chdir / failed\n");
    return 1;
  }
  if(unlink("dots/.") == 0){
    printf(1, "unlink dots/. worked!\n");
    return 1;
  }
  if(unlink("dots/..") == 0){
    printf(1, "unlink dots/.. worked!\n");
    return 1;
  }
  if(unlink("dots") != 0){
    printf(1, "unlink dots failed!\n");
    return 1;
  }
  printf(1, "rmdot ok\n");
  return 0;
}

int
dirfile(void)
{
  int fd;

  printf(1, "dir vs file\n");

  fd = open("dirfile", O_CREATE);
  if(fd < 0){
    printf(1, "create dirfile failed\n");
    return 1;
  }
  close(fd);
  if(chdir("dirfile") == 0){
    printf(1, "chdir dirfile succeeded!\n");
    return 1;
  }
  fd = open("dirfile/xx", 0);
  if(fd >= 0){
    printf(1, "create dirfile/xx succeeded!\n");
    return 1;
  }
  fd = open("dirfile/xx", O_CREATE);
  if(fd >= 0){
    printf(1, "create dirfile/xx succeeded!\n");
    return 1;
  }
  if(mkdir("dirfile/xx") == 0){
    printf(1, "mkdir dirfile/xx succeeded!\n");
    return 1;
  }
  if(unlink("dirfile/xx") == 0){
    printf(1, "unlink dirfile/xx succeeded!\n");
    return 1;
  }
  if(link("README", "dirfile/xx") == 0){
    printf(1, "link to dirfile/xx succeeded!\n");
    return 1;
  }
  if(unlink("dirfile") != 0){
    printf(1, "unlink dirfile failed!\n");
    return 1;
  }

  fd = open(".", O_RDWR);
  if(fd >= 0){
    printf(1, "open . for writing succeeded!\n");
    return 1;
  }
  fd = open(".", 0);
  if(write(fd, "x", 1) > 0){
    printf(1, "write . succeeded!\n");
    return 1;
  }
  close(fd);

  printf(1, "dir vs file OK\n");
  return 0;
}

// test that iput() is called at the end of _namei()
int
iref(void)
{
  int i, fd;

  printf(1, "empty file name\n");

  // the 50 is NINODE
  for(i = 0; i < 50 + 1; i++){
    if(mkdir("irefd") != 0){
      printf(1, "mkdir irefd failed\n");
      return 1;
    }
    if(chdir("irefd") != 0){
      printf(1, "chdir irefd failed\n");
      return 1;
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
  printf(1, "empty file name OK\n");
  return 0;
}

// test that fork fails gracefully
// the forktest binary also does this, but it runs out of proc entries first.
// inside the bigger usertests binary, we run out of memory first.
int
forktest(void)
{
  int n, pid;

  printf(1, "fork test\n");

  for(n=0; n<1000; n++){
    pid = fork();
    if(pid < 0)
      break;
    if(pid == 0)
      exit();
  }
  
  if(n == 1000){
    printf(1, "fork claimed to work 1000 times!\n");
    return 1;
  }
  
  for(; n > 0; n--){
    if(wait() < 0){
      printf(1, "wait stopped early\n");
      return 1;
    }
  }
  
  if(wait() != -1){
    printf(1, "wait got too many\n");
    return 1;
  }
  
  printf(1, "fork test OK\n");
  return 0;
}

int
sbrktest(void)
{
  int fds[2], pid, pids[10], ppid;
  char *a, *b, *c, *lastaddr, *oldbrk, *p, scratch;
  uint amt;

  printf(stdout, "sbrk test\n");
  oldbrk = sbrk(0);

  // can one sbrk() less than a page?
  a = sbrk(0);
  int i;
  for(i = 0; i < 5000; i++){ 
    b = sbrk(1);
    if(b != a){
      printf(stdout, "sbrk test failed %d %x %x\n", i, a, b);
      return 1;
    }
    *b = 1;
    a = b + 1;
  }
  pid = fork();
  if(pid < 0){
    printf(stdout, "sbrk test fork failed\n");
    return 1;
  }
  c = sbrk(1);
  c = sbrk(1);
  if(c != a + 1){
    printf(stdout, "sbrk test failed post-fork\n");
    if(pid > 0)
      return 1;
    else
      exit();
  }
  if(pid == 0)
    exit();
  wait();

  // can one grow address space to something big?
#define BIG (100*1024*1024)
  a = sbrk(0);
  amt = (BIG) - (uint)a;
  p = sbrk(amt);
  if (p != a) { 
    printf(stdout, "sbrk test failed to grow big address space; enough phys mem?\n");
    return 1;
  }
  lastaddr = (char*) (BIG-1);
  *lastaddr = 99;

  // can one de-allocate?
  a = sbrk(0);
  c = sbrk(-4096);
  if(c == (char*)0xffffffff){
    printf(stdout, "sbrk could not deallocate\n");
    return 1;
  }
  c = sbrk(0);
  if(c != a - 4096){
    printf(stdout, "sbrk deallocation produced wrong address, a %x c %x\n", a, c);
    return 1;
  }

  // can one re-allocate that page?
  a = sbrk(0);
  c = sbrk(4096);
  if(c != a || sbrk(0) != a + 4096){
    printf(stdout, "sbrk re-allocation failed, a %x c %x\n", a, c);
    return 1;
  }
  if(*lastaddr == 99){
    // should be zero
    printf(stdout, "sbrk de-allocation didn't really deallocate\n");
    return 1;
  }

  a = sbrk(0);
  c = sbrk(-(sbrk(0) - oldbrk));
  if(c != a){
    printf(stdout, "sbrk downsize failed, a %x c %x\n", a, c);
    return 1;
  }
  
  // can we read the kernel's memory?
  for(a = (char*)(KERNBASE); a < (char*) (KERNBASE+2000000); a += 50000){
    ppid = getpid();
    pid = fork();
    if(pid < 0){
      printf(stdout, "fork failed\n");
      return 1;
    }
    if(pid == 0){
      printf(stdout, "oops could read %x = %x\n", a, *a);
      kill(ppid);
      exit();
    }
    wait();
  }

  // if we run the system out of memory, does it clean up the last
  // failed allocation?
  if(pipe(fds) != 0){
    printf(1, "pipe() failed\n");
    return 1;
  }
  for(i = 0; i < sizeof(pids)/sizeof(pids[0]); i++){
    if((pids[i] = fork()) == 0){
      // allocate a lot of memory
      sbrk(BIG - (uint)sbrk(0));
      write(fds[1], "x", 1);
      // sit around until killed
      for(;;) sleep(1000);
    }
    if(pids[i] != -1)
      read(fds[0], &scratch, 1);
  }
  // if those failed allocations freed up the pages they did allocate,
  // we'll be able to allocate here
  c = sbrk(4096);
  for(i = 0; i < sizeof(pids)/sizeof(pids[0]); i++){
    if(pids[i] == -1)
      continue;
    kill(pids[i]);
    wait();
  }
  if(c == (char*)0xffffffff){
    printf(stdout, "failed sbrk leaked memory\n");
    return 1;
  }

  if(sbrk(0) > oldbrk)
    sbrk(-(sbrk(0) - oldbrk));

  printf(stdout, "sbrk test OK\n");
  return 0;
}

void
validateint(int *p)
{
  int res;
  asm("mov %%esp, %%ebx\n\t"
      "mov %3, %%esp\n\t"
      "int %2\n\t"
      "mov %%ebx, %%esp" :
      "=a" (res) :
      "a" (SYS_sleep), "n" (T_SYSCALL), "c" (p) :
      "ebx");
}

int
validatetest(void)
{
  int hi, pid;
  uint p;

  printf(stdout, "validate test\n");
  hi = 1100*1024;

  for(p = 0; p <= (uint)hi; p += 4096){
    if((pid = fork()) == 0){
      // try to crash the kernel by passing in a badly placed integer
      validateint((int*)p);
      exit();
    }
    sleep(0);
    sleep(0);
    kill(pid);
    wait();

    // try to crash the kernel by passing in a bad string pointer
    if(link("nosuchfile", (char*)p) != -1){
      printf(stdout, "link should not succeed\n");
      return 1;
    }
  }

  printf(stdout, "validate ok\n");
  return 0;
}

// does unintialized data start out zero?
char uninit[10000];
int
bsstest(void)
{
  int i;

  printf(stdout, "bss test\n");
  for(i = 0; i < sizeof(uninit); i++){
    if(uninit[i] != '\0'){
      printf(stdout, "bss test failed\n");
      return 1;
    }
  }
  printf(stdout, "bss test ok\n");
  return 0;
}

// does exec return an error if the arguments
// are larger than a page? or does it write
// below the stack and wreck the instructions/data?
int
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
    printf(stdout, "bigarg test\n");
    exec("echo", args);
    printf(stdout, "bigarg test ok\n");
    fd = open("bigarg-ok", O_CREATE);
    close(fd);
    exit();
  } else if(pid < 0){
    printf(stdout, "bigargtest: fork failed\n");
    return 1;
  }
  wait();
  fd = open("bigarg-ok", 0);
  if(fd < 0){
    printf(stdout, "bigarg test failed!\n");
    return 1;
  }
  close(fd);
  unlink("bigarg-ok");
  return 0;
}

unsigned long randstate = 1;
unsigned int
rand()
{
  randstate = randstate * 1664525 + 1013904223;
  return randstate;
}


int
testuptime(void)
{
  int t1;
  printf(1, "uptime test\n");
  t1 = uptime();
  if(t1 < 0){
    return 1;
  }  
  printf(1, "uptime ok\n");
  return 0;
}

int
sleeplong(void)
{
  printf(1, "sleeplong test\n");
  sleep(100000000);
  printf(1, "sleeplong ok\n");
  return 0;
}

int
sleepshort(void)
{
  printf(1, "sleepshort test\n");
  sleep(0);
  printf(1, "sleepshort ok\n");
  return 0;
}

int
getpidtest(void)
{
  int pid = -1;

  printf(1, "getpid test\n");
  pid = getpid();
  if(pid <= 0){
    printf(1, "getpid() returned %d and failed\n", pid);
    return -1;
  }

  pid = fork();
  if(pid == 0){
    // This is the child
    pid = getpid();
    if(pid <= 0){
      printf(1, "getpid() returned %d and failed\n", pid);
      exit();
    }
    exit();
  }
  else{
    wait();
  }

  printf(1, "getpid ok\n");
  return 0;
}

int
duptest(void)
{
  int fd;
  // int res, i;

  printf(1, "dup test\n");

  unlink("dup_test");
  fd = open("dup_test", O_CREATE);

  // for(i = 0; i < 100; i++){
  //   res = dup(fd);
  //   printf(1, "res: %d\n", res);
  //   if((int)res == 0xFFFFFFFF){
  //     printf(1, "dup() failed\n");
  //     return 1;
  //   }
  // }

  close(fd);
  unlink("dup_test");

  fd = dup(fd);
  if(fd >= 0){
    printf(1, "dup on close file works - fail\n");
    return 1;
  }
  printf(1, "dup ok\n");
  return 0;

}

int
simpleread(void)
{
  int fd, res;

  printf(1, "simpleread test\n");
  unlink("readtest");

  fd = open("readtest",  O_CREATE|O_WRONLY);
  if(fd < 0){
    printf(1, "open() failed\n");
    return 1;
  }

  res = write(fd, "aaaa", 5);
  if(res != 5){
    printf(1, "write() failed\n");
    return 1;
  }

  close(fd);

  fd = open("readtest", O_RDONLY);
  if(fd < 0){
    printf(1, "open() failed\n");
    return 1;
  }

  res = read(fd, buf, 5);
  if(res != 5){
    printf(1, "read() failed\n");
    return 1;
  }
  close(fd);
  unlink("readtest");
  if(buf[0] == 'a' && buf[1] == 'a' && buf[2] == 'a' && buf[3] == 'a' && buf[4] == '\0'){
    printf(1, "simpleread ok\n");
    return 0;
  }
  printf(1, "read() fail\n");
  return 1;
}

int
badfdread(void)
{
  int res;

  printf(1, "badfdread test\n");
  close(1005);
  res = read(1005, buf, 5);
  if(res != -1){
    printf(1, "read() success on closed fd! fail\n");
    return 1;
  }

  printf(1, "badfdread ok\n");
  return 0;
}

int
simplemknod(void)
{
  int fd;

  printf(1, "simplemknod test\n");

  mknod("newdevice", 2, 2);

  if((fd = open("newdevice", 0)) < 0){
    printf(1, "open() fail\n");
    return 1;
  }

  close(fd);
  unlink("newdevice");

  printf(1, "simplemknod ok\n");
  return 0;
}

int
simplefstat(void){
  int fd, res;
  struct stat st;

  printf(1, "simplefstat test\n");

  unlink("fstattest");
  fd = open("fstattest", O_CREATE);
  if(fd <= 0){
    printf(1, "open() failed\n");
    return 1;
  }

  res = fstat(fd, &st);
  if(res != 0){
    printf(1, "fstat() failed\n");
    return 1;
  }
  if(st.type != T_FILE || st.size != 0){
    printf(1, "fstat() returned false data\n");
    return 1;
  }

  close(fd);
  unlink("fstattest");
  printf(1, "simplefstat ok\n");
  return 0;

}
int
badfdfstat(void)
{
  int res;
  struct stat st;
  
  printf(1, "badfdfstat test\n");

  close(1005);
  res = fstat(1005, &st);
  if(res == 0){
    printf(1, "fstat() on bad fd work! fail\n");
    return 1;
  }

  printf(1, "badfdfstat ok\n");
  return 0;
}

int
waittest(void)
{
  int pid, pid2;

  printf(1, "wait test\n");
  pid = fork();
  if(pid < 0){
    printf(1, "fork() failed\n");
    return 1;
  }

  if(pid == 0){
    exit();
  }

  if(pid > 0){
    pid2 = wait();
    if(pid != pid2){
      printf(1, "wait() failed\n");
      return 1;
    }
  }

  printf(1, "waittest ok\n");
  return 0;
}


// try to find any races between exit and wait
int
exitwait(void)
{
  int i, pid;

  printf(1, "exitwait test\n");

  for(i = 0; i < 100; i++){
    pid = fork();
    if(pid < 0){
      printf(1, "fork failed\n");
      return 1;
    }
    if(pid){
      if(wait() != pid){
        printf(1, "wait wrong pid\n");
        return 1;
      }
    } else {
      exit();
    }
  }
  printf(1, "exitwait ok\n");
  return 0;
}

// does exit() call iput(p->cwd) in a transaction?
int
exitiputtest(void)
{
  int pid;

  printf(stdout, "exitiput test\n");

  pid = fork();
  if(pid < 0){
    printf(stdout, "fork failed\n");
    return 1;
  }
  if(pid == 0){
    if(mkdir("iputdir") < 0){
      printf(stdout, "mkdir failed\n");
      exit();
    }
    if(chdir("iputdir") < 0){
      printf(stdout, "child chdir failed\n");
      exit();
    }
    if(unlink("../iputdir") < 0){
      printf(stdout, "unlink ../iputdir failed\n");
      exit();
    }
    exit();
  }
  wait();
  printf(stdout, "exitiput test ok\n");
  return 0;
}

int
openfileexit(void)
{
  int pid;

  printf(1, "openfileexit test\n");

  pid = fork();
  if(pid == 0){
    unlink("testfile");
    if(open("testfile", O_CREATE) <= 0){
      printf(1, "open() failed\n");
      exit();
    }
    exit();
  }
  else{
    wait();
  }
  printf(1, "openfileexit() ok\n");
  return 0;
}

int
killtest(void)
{
  int hi, pid;
  uint p;

  printf(stdout, "kill test\n");
  hi = 1100*1024;

  for(p = 0; p <= (uint)hi; p += 4096){
    if((pid = fork()) == 0){
      sleep(10);
      exit();
    }

    // Kill a nonexisting process
    sleep(0);
    sleep(0);

    // Kill the process
    sleep(0);
    sleep(0);

    // Kill the process again
    sleep(0);
    sleep(0);

    // Kill a negative pid
    sleep(0);
    sleep(0);

    wait();

  }

  printf(stdout, "kill ok\n");
  return 0;
}

int
mkdir_chdir_unlink(void)
{
  printf(stdout, "mkdir_chdir_unlink test\n");

  //Create the same directory twice
  if(mkdir("dir0") < 0){
      printf(stdout, "mkdir failed\n");
      return 1;
  }
  if(mkdir("dir0") < 0){
      printf(stdout, "mkdir dir1 twice failed\n");
  }

  //Create an impossible directory
  if(mkdir("dir1/dir0") < 0){
      printf(stdout, "mkdir dir1/dir0 failed\n");
  }

  //Change dir to an impossible directory
  if(chdir("dir1/dir0") < 0){
      printf(stdout, "chdir dir1/dir0 failed\n");
  }

  //Change dir to a legit directory
  if(chdir("dir0") < 0){
      printf(stdout, "chdir dir0 failed\n");
      return 1;
  }

  //Change dir to a legit twice
  if(chdir("dir0") < 0){
      printf(stdout, "chdir dir0 twice failed\n");
  }

  //Change dir to a legit directory
  if(chdir("..") < 0){
      printf(stdout, "chdir .. failed\n");
      return 1;
  }

  //Unlink a legit directory
  if(unlink("dir0") < 0){
      printf(stdout, "unlink dir0 failed\n");
      return 1;
  }

  //Unlink the same dir twice
  if(unlink("dir0") < 0){
      printf(stdout, "unlink dir0 twice failed\n");
  }

  //Unlink an impossible directory
  if(unlink("dir1/dir0") < 0){
      printf(stdout, "unlink dir1/dir0 failed\n");
  }

  printf(stdout, "mkdir_chdir_unlink test ok\n");
  return 0;
}

int
exectest2(void)
{
    int pid;
    printf(stdout, "exec test\n");

    pid = fork();
    if(pid == 0){
      if(exec("echo", echoargv1) < 0){
          printf(stdout, "exec echo failed\n");
          exit();
      }
      exit();

    }
    else{
      wait();
    }

    pid = fork();
    if(pid == 0){
      printf(stdout, "impossible exec test\n");
      if(exec("echa", echoargv1) < 0){
          printf(stdout, "exec echa failed\n");
          exit();
      }
      exit();
    }
    else{
      wait();
    }
    printf(stdout, "exec test done\n");
    return 0;
}

int
opentest2(void)
{
    int fd;

    printf(1, "open tests\n");

    fd = open("dirfile", O_CREATE);
    if(fd < 0){
        printf(1, "create dirfile failed\n");
        return 1;
    }
    close(fd);

    fd = open("dir1/dirfile", O_CREATE);
    if(fd < 0){
        printf(1, "create dir1/dirfile failed\n");
    }

    if(unlink("dirfile") != 0){
        printf(1, "unlink dirfile failed!\n");
        return 1;
    }

    printf(1, "open tests done\n");
    return 0;
}

int
main(int argc, char *argv[])
{
  int status = 0;
  printf(1, "usertests starting\n");

  //uptime() tests 

  // Make sure the gap between uptime() calls is correct
  status = status + testuptime();

  // sleep() tests

  // sleep() for a long time
  // status = status + sleeplong(); // This takes a long time so it's commented out by default.

  // sleep() for a short time
  status = status + sleepshort();
  
  // getpid() tests
  // Check getpid() in child process + parent process
  status = status + getpidtest();


  // read() tests
  
  // read() from file
  status = status + simpleread();

  // read() from bad file descriptor
  status = status + badfdread();
  
  // write() tests

  // write() to a file, then read it and make sure it worked
  status = status + writetest();

  // write() a lot of data to a file
  status = status + writetest1();

  // write() many times from different processes
  status = status + fourfiles();

  // two processes write() to the same file descriptor
  // is the offset shared? does inode locking work?
  status = status + sharedfd();

  // test writes that are larger than the log.
  status = status + bigwrite();

  // mknod() tests
  // create a device
  status = status + simplemknod();

  // fstat() tests

  status = status + simplefstat();

  // fstat() on bad file descriptor
  status = status + badfdfstat();
  
  // link() tests
  status = status + linktest();

  // dup() test
  status = status + duptest();
  
  // pipe() test

  // write/read to pipe from parent and child processes
  status = status + pipe1();

  // fork() tests
  
  // fork() 1000 times
  status = status + forktest();

  // wait() test
  status = status + waittest();


  // exit() tests

  // does exit() call iput(p->cwd) in a transaction?
  status = status + exitiputtest();

  // try to find any races between exit and wait
  status = status + exitwait();

  // exit() with open file
  status = status + openfileexit();
  // kill() tests
  status = status + killtest();

  // mkdir() test
  status = status + mkdir_chdir_unlink();

  // exec() test
  status = status + exectest2();

  // open() test
  status = status + opentest2();

  status = status + createdelete();
  status = status + linkunlink();
  status = status + concreate();
  status = status + fourfiles();
  status = status + sharedfd();

  status = status + bigargtest();
  status = status + bigwrite();
  status = status + bigargtest();
  status = status + bsstest();
  status = status + sbrktest();
  status = status + validatetest();

  status = status + opentest();
  status = status + writetest();
  status = status + writetest1();
  status = status + createtest();

  status = status + openiputtest();
  status = status + exitiputtest();
  status = status + iputtest();

  status = status + mem();
  status = status + pipe1();
  status = status + preempt();
  status = status + exitwait();

  status = status + rmdot();
  status = status + fourteen();
  status = status + bigfile();
  status = status + subdir();
  status = status + linktest();
  status = status + unlinkread();
  status = status + dirfile();
  status = status + iref();
  status = status + forktest();
  status = status + bigdir(); // slow

  if(status == 0){
    printf(1, "usertests done!\n");
  }
  else{
    printf(1, "usertests failed\n");
  }
  exit();
}