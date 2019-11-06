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

void
failexit(const char * const msg)
{
  printf(1, "!! FAILED %s\n", msg);
  exit();
}

// does chdir() call iput(p->cwd) in a transaction?
void
iputtest(void)
{
  printf(1, "iput test\n");

  if(mkdir("iputdir") < 0){
    failexit("mkdir");
  }
  if(chdir("iputdir") < 0){
    failexit("chdir iputdir");
  }
  if(unlink("../iputdir") < 0){
    failexit("unlink ../iputdir");
  }
  if(chdir("/") < 0){
    failexit("chdir /");
  }
  printf(1, "iput test ok\n");
}

// does exit() call iput(p->cwd) in a transaction?
void
exitiputtest(void)
{
  int pid;

  printf(1, "exitiput test\n");

  pid = fork();
  if(pid < 0){
    failexit("fork");
  }
  if(pid == 0){
    if(mkdir("iputdir") < 0){
      failexit("mkdir");
    }
    if(chdir("iputdir") < 0){
      failexit("child chdir");
    }
    if(unlink("../iputdir") < 0){
      failexit("unlink ../iputdir");
    }
    exit();
  }
  wait();
  printf(1, "exitiput test ok\n");
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

  printf(1, "openiput test\n");
  if(mkdir("oidir") < 0){
    failexit("mkdir oidir");
  }
  pid = fork();
  if(pid < 0){
    failexit("fork");
  }
  if(pid == 0){
    int fd = open("oidir", O_RDWR);
    if(fd >= 0){
      failexit("open directory for write succeeded");
    }
    exit();
  }
  sleep(1);
  if(unlink("oidir") != 0){
    failexit("unlink");
  }
  wait();
  printf(1, "openiput test ok\n");
}

// simple file system tests

void
opentest(void)
{
  int fd;

  printf(1, "open test\n");
  fd = open("echo", 0);
  if(fd < 0){
    failexit("open echo");
  }
  close(fd);
  fd = open("doesnotexist", 0);
  if(fd >= 0){
    failexit("open doesnotexist succeeded!");
  }
  printf(1, "open test ok\n");
}

void
writetest(void)
{
  int fd;
  int i;

  printf(1, "small file test\n");
  fd = open("small", O_CREATE|O_RDWR);
  if(fd < 0){
    failexit("error: creat small");
  }
  for(i = 0; i < 100; i++){
    if(write(fd, "aaaaaaaaaa", 10) != 10){
      printf(1, "error: write aa %d new file failed\n", i);
      exit();
    }
    if(write(fd, "bbbbbbbbbb", 10) != 10){
      printf(1, "error: write bb %d new file failed\n", i);
      exit();
    }
  }
  close(fd);
  fd = open("small", O_RDONLY);
  if(fd < 0){
    failexit("error: open small");
  }
  i = read(fd, buf, 2000);
  if(i != 2000){
    failexit("read");
  }
  close(fd);

  if(unlink("small") < 0){
    failexit("unlink small");
    exit();
  }
  printf(1, "small file test ok\n");
}

void
writetest1(void)
{
  int i, fd, n;

  printf(1, "big files test\n");

  fd = open("big", O_CREATE|O_RDWR);
  if(fd < 0){
    failexit("error: creat big");
  }

  for(i = 0; i < MAXFILE; i++){
    ((int*)buf)[0] = i;
    if(write(fd, buf, 512) != 512){
      failexit("error: write big file");
    }
  }

  close(fd);

  fd = open("big", O_RDONLY);
  if(fd < 0){
    failexit("error: open big");
  }

  n = 0;
  for(;;){
    i = read(fd, buf, 512);
    if(i == 0){
      if(n == MAXFILE - 1){
        printf(1, "read only %d blocks from big. failed", n);
        exit();
      }
      break;
    } else if(i != 512){
      printf(1, "read failed %d\n", i);
      exit();
    }
    if(((int*)buf)[0] != n){
      printf(1, "read content of block %d is %d. failed\n",
             n, ((int*)buf)[0]);
      exit();
    }
    n++;
  }
  close(fd);
  if(unlink("big") < 0){
    failexit("unlink big");
    exit();
  }
  printf(1, "big files ok\n");
}

void
createtest(void)
{
  int i, fd;

  printf(1, "many creates, followed by unlink test\n");

  name[0] = 'a';
  name[2] = '\0';
  for(i = 0; i < 52; i++){
    name[1] = '0' + i;
    fd = open(name, O_CREATE|O_RDWR);
    close(fd);
  }
  for(i = 0; i < 52; i++){
    name[1] = '0' + i;
    unlink(name);
  }
  for(i = 0; i < 52; i++){
    name[1] = '0' + i;
    fd = open(name, O_RDWR);
    if(fd >= 0) {
      failexit("open should fail.");
    }
  }

  printf(1, "many creates, followed by unlink; ok\n");
}

void dirtest(void)
{
  printf(1, "mkdir test\n");

  if(mkdir("dir0") < 0){
    failexit("mkdir");
  }

  if(chdir("dir0") < 0){
    failexit("chdir dir0");
  }

  if(chdir("..") < 0){
    failexit("chdir ..");
  }

  if(unlink("dir0") < 0){
    failexit("unlink dir0");
  }
  printf(1, "mkdir test ok\n");
}

void
exectest(void)
{
  printf(1, "exec test\n");
  if(exec("echo", echoargv) < 0){
    failexit("exec echo");
  }
  printf(1, "exec test ok\n");
}

void
nullptr(void)
{
  printf(1, "null pointer test\n");
  printf(1, "expect one killed process\n");
  int ppid = getpid();
  if (fork() == 0) {
    *(addr_t *)(0) = 10;
    printf(1, "can write to unmapped page 0, failed");
    kill(ppid);
    exit();
  } else {
    wait();
  }
  printf(1, "null pointer test ok\n");
}

// simple fork and pipe read/write

void
pipe1(void)
{
  int fds[2], pid;
  int seq, i, n, cc, total;

  if(pipe(fds) != 0){
    failexit("pipe()");
  }
  pid = fork();
  seq = 0;
  if(pid == 0){
    close(fds[0]);
    for(n = 0; n < 5; n++){
      for(i = 0; i < 1033; i++)
        buf[i] = seq++;
      if(write(fds[1], buf, 1033) != 1033){
        failexit("pipe1 oops 1");
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
          failexit("pipe1 oops 2");
          return;
        }
      }
      total += n;
      cc = cc * 2;
      if(cc > sizeof(buf))
        cc = sizeof(buf);
    }
    if(total != 5 * 1033){
      printf(1, "pipe1 oops 3 total %d\n", total);
      exit();
    }
    close(fds[0]);
    wait();
  } else {
    failexit("fork()");
  }
  printf(1, "pipe1 ok\n");
}

// meant to be run w/ at most two CPUs
void
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
    return;
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
}

// try to find any races between exit and wait
void
exitwait(void)
{
  int i, pid;

  for(i = 0; i < 100; i++){
    pid = fork();
    if(pid < 0){
      printf(1, "fork");
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
  printf(1, "exitwait ok\n");
}

void
mem(void)
{
  void *m1, *m2;
  int pid, ppid;

  printf(1, "mem test\n");
  ppid = getpid();
  if((pid = fork()) == 0){
    m1 = 0;
    while((m2 = malloc(100001)) != 0){
      //printf(1, "m2 %p\n", m2);
      *(void**)m2 = m1;
      m1 = m2;
    }
    printf(1, "alloc ended\n");
    while(m1){
      m2 = *(void**)m1;
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
  }
}

// More file system tests

// two processes write to the same file descriptor
// is the offset shared? does inode locking work?
void
sharedfd(void)
{
  int fd, pid, i, n, nc, np;
  char buf[10];

  printf(1, "sharedfd test\n");

  unlink("sharedfd");
  fd = open("sharedfd", O_CREATE|O_RDWR);
  if(fd < 0){
    printf(1, "fstests: cannot open sharedfd for writing");
    return;
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
  if(nc == 10000 && np == 10000){
    printf(1, "sharedfd ok\n");
  } else {
    printf(1, "sharedfd oops %d %d\n", nc, np);
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

  printf(1, "fourfiles test\n");

  for(pi = 0; pi < 4; pi++){
    fname = names[pi];
    unlink(fname);

    pid = fork();
    if(pid < 0){
      failexit("fork");
    }

    if(pid == 0){
      fd = open(fname, O_CREATE | O_RDWR);
      if(fd < 0){
        failexit("create");
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
          failexit("wrong char");
        }
      }
      total += n;
    }
    close(fd);
    if(total != 12*500){
      printf(1, "wrong length %d\n", total);
      exit();
    }
    unlink(fname);
  }

  printf(1, "fourfiles ok\n");
}

// four processes create and delete different files in same directory
void
createdelete(void)
{
  enum { N = 20 };
  int pid, i, fd, pi;
  char name[32];

  printf(1, "createdelete test\n");

  for(pi = 0; pi < 4; pi++){
    pid = fork();
    if(pid < 0){
      failexit("fork");
    }

    if(pid == 0){
      name[0] = 'p' + pi;
      name[2] = '\0';
      for(i = 0; i < N; i++){
        name[1] = '0' + i;
        fd = open(name, O_CREATE | O_RDWR);
        if(fd < 0){
          failexit("create");
        }
        close(fd);
        if(i > 0 && (i % 2 ) == 0){
          name[1] = '0' + (i / 2);
          if(unlink(name) < 0){
            failexit("unlink");
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
        exit();
      } else if((i >= 1 && i < N/2) && fd >= 0){
        printf(1, "oops createdelete %s did exist\n", name);
        exit();
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
}

// can I unlink a file and still read it?
void
unlinkread(void)
{
  int fd, fd1;

  printf(1, "unlinkread test\n");
  fd = open("unlinkread", O_CREATE | O_RDWR);
  if(fd < 0){
    failexit("create unlinkread");
  }
  write(fd, "hello", 5);
  close(fd);

  fd = open("unlinkread", O_RDWR);
  if(fd < 0){
    failexit("open unlinkread");
  }
  if(unlink("unlinkread") != 0){
    failexit("unlink unlinkread");
  }

  fd1 = open("unlinkread", O_CREATE | O_RDWR);
  write(fd1, "yyy", 3);
  close(fd1);

  if(read(fd, buf, sizeof(buf)) != 5){
    failexit("unlinkread read failed");
  }
  if(buf[0] != 'h'){
    failexit("unlinkread wrong data");
  }
  if(write(fd, buf, 10) != 10){
    failexit("unlinkread write");
  }
  close(fd);
  unlink("unlinkread");
  printf(1, "unlinkread ok\n");
}

void
linktest(void)
{
  int fd;

  printf(1, "linktest\n");

  unlink("lf1");
  unlink("lf2");

  fd = open("lf1", O_CREATE|O_RDWR);
  if(fd < 0){
    failexit("create lf1");
  }
  if(write(fd, "hello", 5) != 5){
    failexit("write lf1");
  }
  close(fd);

  if(link("lf1", "lf2") < 0){
    failexit("link lf1 lf2");
  }
  unlink("lf1");

  if(open("lf1", 0) >= 0){
    failexit("unlinked lf1 but it is still there!");
  }

  fd = open("lf2", 0);
  if(fd < 0){
    failexit("open lf2");
  }
  if(read(fd, buf, sizeof(buf)) != 5){
    failexit("read lf2");
  }
  close(fd);

  if(link("lf2", "lf2") >= 0){
    failexit("link lf2 lf2 succeeded! oops");
  }

  unlink("lf2");
  if(link("lf2", "lf1") >= 0){
    failexit("link non-existant succeeded! oops");
  }

  if(link(".", "lf1") >= 0){
    failexit("link . lf1 succeeded! oops");
  }

  printf(1, "linktest ok\n");
}

// test concurrent create/link/unlink of the same file
void
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
        exit();
      }
      if(fa[i]){
        printf(1, "concreate duplicate file %s\n", de.name);
        exit();
      }
      fa[i] = 1;
      n++;
    }
  }
  close(fd);

  if(n != 40){
    failexit("concreate not enough files in directory listing");
  }

  for(i = 0; i < 40; i++){
    file[1] = '0' + i;
    pid = fork();
    if(pid < 0){
      failexit("fork");
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
}

// another concurrent link/unlink/create test,
// to look for deadlocks.
void
linkunlink()
{
  int pid, i;

  printf(1, "linkunlink test\n");

  unlink("x");
  pid = fork();
  if(pid < 0){
    failexit("fork");
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
}

// directory that uses indirect blocks
void
bigdir(void)
{
  int i, fd;
  char name[10];

  printf(1, "bigdir test\n");
  unlink("bd");

  fd = open("bd", O_CREATE);
  if(fd < 0){
    failexit("bigdir create");
  }
  close(fd);

  for(i = 0; i < 500; i++){
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    if(link("bd", name) != 0){
      failexit("bigdir link");
    }
  }

  unlink("bd");
  for(i = 0; i < 500; i++){
    name[0] = 'x';
    name[1] = '0' + (i / 64);
    name[2] = '0' + (i % 64);
    name[3] = '\0';
    if(unlink(name) != 0){
      failexit("bigdir unlink failed");
    }
  }

  printf(1, "bigdir ok\n");
}

void
subdir(void)
{
  int fd, cc;

  printf(1, "subdir test\n");

  unlink("ff");
  if(mkdir("dd") != 0){
    failexit("subdir mkdir dd");
  }

  fd = open("dd/ff", O_CREATE | O_RDWR);
  if(fd < 0){
    failexit("create dd/ff");
  }
  write(fd, "ff", 2);
  close(fd);

  if(unlink("dd") >= 0){
    failexit("unlink dd (non-empty dir) succeeded!");
  }

  if(mkdir("/dd/dd") != 0){
    failexit("subdir mkdir dd/dd");
  }

  fd = open("dd/dd/ff", O_CREATE | O_RDWR);
  if(fd < 0){
    failexit("create dd/dd/ff");
  }
  write(fd, "FF", 2);
  close(fd);

  fd = open("dd/dd/../ff", 0);
  if(fd < 0){
    failexit("open dd/dd/../ff");
  }
  cc = read(fd, buf, sizeof(buf));
  if(cc != 2 || buf[0] != 'f'){
    failexit("dd/dd/../ff wrong content");
  }
  close(fd);

  if(link("dd/dd/ff", "dd/dd/ffff") != 0){
    failexit("link dd/dd/ff dd/dd/ffff");
  }

  if(unlink("dd/dd/ff") != 0){
    failexit("unlink dd/dd/ff");
  }
  if(open("dd/dd/ff", O_RDONLY) >= 0){
    failexit("open (unlinked) dd/dd/ff succeeded");
  }

  if(chdir("dd") != 0){
    failexit("chdir dd");
  }
  if(chdir("dd/../../dd") != 0){
    failexit("chdir dd/../../dd");
  }
  if(chdir("dd/../../../dd") != 0){
    failexit("chdir dd/../../dd");
  }
  if(chdir("./..") != 0){
    failexit("chdir ./..");
  }

  fd = open("dd/dd/ffff", 0);
  if(fd < 0){
    failexit("open dd/dd/ffff");
  }
  if(read(fd, buf, sizeof(buf)) != 2){
    failexit("read dd/dd/ffff wrong len");
  }
  close(fd);

  if(open("dd/dd/ff", O_RDONLY) >= 0){
    failexit("open (unlinked) dd/dd/ff succeeded");
  }

  if(open("dd/ff/ff", O_CREATE|O_RDWR) >= 0){
    failexit("create dd/ff/ff succeeded");
  }
  if(open("dd/xx/ff", O_CREATE|O_RDWR) >= 0){
    failexit("create dd/xx/ff succeeded");
  }
  if(open("dd", O_CREATE) >= 0){
    failexit("create dd succeeded");
  }
  if(open("dd", O_RDWR) >= 0){
    failexit("open dd rdwr succeeded");
  }
  if(open("dd", O_WRONLY) >= 0){
    failexit("open dd wronly succeeded");
  }
  if(link("dd/ff/ff", "dd/dd/xx") == 0){
    failexit("link dd/ff/ff dd/dd/xx succeeded");
  }
  if(link("dd/xx/ff", "dd/dd/xx") == 0){
    failexit("link dd/xx/ff dd/dd/xx succeededn");
  }
  if(link("dd/ff", "dd/dd/ffff") == 0){
    failexit("link dd/ff dd/dd/ffff succeeded");
  }
  if(mkdir("dd/ff/ff") == 0){
    failexit("mkdir dd/ff/ff succeeded");
  }
  if(mkdir("dd/xx/ff") == 0){
    failexit("mkdir dd/xx/ff succeeded");
  }
  if(mkdir("dd/dd/ffff") == 0){
    failexit("mkdir dd/dd/ffff succeeded");
  }
  if(unlink("dd/xx/ff") == 0){
    failexit("unlink dd/xx/ff succeeded");
  }
  if(unlink("dd/ff/ff") == 0){
    failexit("unlink dd/ff/ff succeeded");
  }
  if(chdir("dd/ff") == 0){
    failexit("chdir dd/ff succeeded");
  }
  if(chdir("dd/xx") == 0){
    failexit("chdir dd/xx succeeded");
  }

  if(unlink("dd/dd/ffff") != 0){
    failexit("unlink dd/dd/ff");
  }
  if(unlink("dd/ff") != 0){
    failexit("unlink dd/ff");
  }
  if(unlink("dd") == 0){
    failexit("unlink non-empty dd succeeded");
  }
  if(unlink("dd/dd") < 0){
    failexit("unlink dd/dd");
  }
  if(unlink("dd") < 0){
    failexit("unlink dd");
  }

  printf(1, "subdir ok\n");
}

// test writes that are larger than the log.
void
bigwrite(void)
{
  int fd, sz;

  printf(1, "bigwrite test\n");

  unlink("bigwrite");
  for(sz = 499; sz < 12*512; sz += 471){
    fd = open("bigwrite", O_CREATE | O_RDWR);
    if(fd < 0){
      failexit("cannot create bigwrite");
    }
    int i;
    for(i = 0; i < 2; i++){
      int cc = write(fd, buf, sz);
      if(cc != sz){
        printf(1, "write(%d) ret %d\n", sz, cc);
        exit();
      }
    }
    close(fd);
    unlink("bigwrite");
  }

  printf(1, "bigwrite ok\n");
}

void
bigfile(void)
{
  int fd, i, total, cc;

  printf(1, "bigfile test\n");

  unlink("bigfile");
  fd = open("bigfile", O_CREATE | O_RDWR);
  if(fd < 0){
    failexit("cannot create bigfile");
  }
  for(i = 0; i < 20; i++){
    memset(buf, i, 600);
    if(write(fd, buf, 600) != 600){
      failexit("write bigfile");
    }
  }
  close(fd);

  fd = open("bigfile", 0);
  if(fd < 0){
    failexit("cannot open bigfile");
  }
  total = 0;
  for(i = 0; ; i++){
    cc = read(fd, buf, 300);
    if(cc < 0){
      failexit("read bigfile");
    }
    if(cc == 0)
      break;
    if(cc != 300){
      failexit("short read bigfile");
    }
    if(buf[0] != i/2 || buf[299] != i/2){
      failexit("read bigfile wrong data");
    }
    total += cc;
  }
  close(fd);
  if(total != 20*600){
    failexit("read bigfile wrong total");
  }
  unlink("bigfile");

  printf(1, "bigfile test ok\n");
}

void
fourteen(void)
{
  int fd;

  // DIRSIZ is 14.
  printf(1, "fourteen test\n");

  if(mkdir("12345678901234") != 0){
    failexit("mkdir 12345678901234");
  }
  if(mkdir("12345678901234/123456789012345") != 0){
    failexit("mkdir 12345678901234/123456789012345");
  }
  fd = open("123456789012345/123456789012345/123456789012345", O_CREATE);
  if(fd < 0){
    failexit("create 123456789012345/123456789012345/123456789012345");
  }
  close(fd);
  fd = open("12345678901234/12345678901234/12345678901234", 0);
  if(fd < 0){
    failexit("open 12345678901234/12345678901234/12345678901234");
  }
  close(fd);

  if(mkdir("12345678901234/12345678901234") == 0){
    failexit("mkdir 12345678901234/12345678901234 succeeded");
  }
  if(mkdir("123456789012345/12345678901234") == 0){
    failexit("mkdir 12345678901234/123456789012345 succeeded");
  }

  printf(1, "fourteen ok\n");
}

void
rmdot(void)
{
  printf(1, "rmdot test\n");
  if(mkdir("dots") != 0){
    failexit("mkdir dots");
  }
  if(chdir("dots") != 0){
    failexit("chdir dots");
  }
  if(unlink(".") == 0){
    failexit("rm . worked");
  }
  if(unlink("..") == 0){
    failexit("rm .. worked");
  }
  if(chdir("/") != 0){
    failexit("chdir /");
  }
  if(unlink("dots/.") == 0){
    failexit("unlink dots/. worked");
  }
  if(unlink("dots/..") == 0){
    failexit("unlink dots/.. worked");
  }
  if(unlink("dots") != 0){
    failexit("unlink dots");
  }
  printf(1, "rmdot ok\n");
}

void
dirfile(void)
{
  int fd;

  printf(1, "dir vs file\n");

  fd = open("dirfile", O_CREATE);
  if(fd < 0){
    failexit("create dirfile");
  }
  close(fd);
  if(chdir("dirfile") == 0){
    failexit("chdir dirfile succeeded");
  }
  fd = open("dirfile/xx", 0);
  if(fd >= 0){
    failexit("create dirfile/xx succeeded");
  }
  fd = open("dirfile/xx", O_CREATE);
  if(fd >= 0){
    failexit("create dirfile/xx succeeded");
  }
  if(mkdir("dirfile/xx") == 0){
    failexit("mkdir dirfile/xx succeeded");
  }
  if(unlink("dirfile/xx") == 0){
    failexit("unlink dirfile/xx succeeded");
  }
  if(link("README", "dirfile/xx") == 0){
    failexit("link to dirfile/xx succeeded");
  }
  if(unlink("dirfile") != 0){
    failexit("unlink dirfile");
  }

  fd = open(".", O_RDWR);
  if(fd >= 0){
    failexit("open . for writing succeeded");
  }
  fd = open(".", 0);
  if(write(fd, "x", 1) > 0){
    failexit("write . succeeded");
  }
  close(fd);

  printf(1, "dir vs file OK\n");
}

// test that iput() is called at the end of _namei()
void
iref(void)
{
  int i, fd;

  printf(1, "empty file name\n");

  // the 50 is NINODE
  for(i = 0; i < 50 + 1; i++){
    if(mkdir("irefd") != 0){
      failexit("mkdir irefd");
    }
    if(chdir("irefd") != 0){
      failexit("chdir irefd");
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
}

// test that fork fails gracefully
// the forktest binary also does this, but it runs out of proc entries first.
// inside the bigger usertests binary, we run out of memory first.
void
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
    failexit("fork claimed to work 1000 times");
  }

  for(; n > 0; n--){
    if(wait() < 0){
      failexit("wait stopped early");
    }
  }

  if(wait() != -1){
    failexit("wait got too many");
  }

  printf(1, "fork test OK\n");
}

void
sbrktest(void)
{
  int fds[2], pid, pids[10], ppid;
  char *a, *b, *c, *lastaddr, *oldbrk, *p, scratch;
  uint amt;

  printf(1, "sbrk test\n");
  oldbrk = sbrk(0);

  // can one sbrk() less than a page?
  a = sbrk(0);
  int i;
  for(i = 0; i < 5000; i++){
    b = sbrk(1);
    if(b != a){
      printf(1, "sbrk test failed %d %p %p\n", i, a, b);
      exit();
    }
    *b = 1;
    a = b + 1;
  }
  pid = fork();
  if(pid < 0){
    failexit("sbrk test fork");
  }
  c = sbrk(1);
  c = sbrk(1);
  if(c != a + 1){
    failexit("sbrk test failed post-fork");
  }
  if(pid == 0)
    exit();
  wait();

  // can one grow address space to something big?
#define BIG (100*1024*1024)
  a = sbrk(0);
  amt = (BIG) - (addr_t)a;
  p = sbrk(amt);
  if (p != a) {
    failexit("sbrk test failed to grow big address space; enough phys mem?");
  }
  lastaddr = (char*) (BIG-1);
  *lastaddr = 99;

  // can one de-allocate?
  a = sbrk(0);
  c = sbrk(-4096);
  if(c == (char*)0xffffffff){
    failexit("sbrk could not deallocate");
  }
  c = sbrk(0);
  if(c != a - 4096){
    printf(1, "sbrk deallocation produced wrong address, a %p c %p\n", a, c);
    exit();
  }

  // can one re-allocate that page?
  a = sbrk(0);
  c = sbrk(4096);
  if(c != a || sbrk(0) != a + 4096){
    printf(1, "sbrk re-allocation failed, a %p c %p\n", a, c);
    exit();
  }
  if(*lastaddr == 99){
    // should be zero
    failexit("sbrk de-allocation didn't really deallocate");
  }

  a = sbrk(0);
  c = sbrk(-(sbrk(0) - oldbrk));
  if(c != a){
    printf(1, "sbrk downsize failed, a %p c %p\n", a, c);
    exit();
  }

  printf(1, "expecting 10 killed processes:\n");
  // can we read the kernel's memory?
  for(a = (char*)(KERNBASE); a < (char*) (KERNBASE+1000000); a += 100000){
    ppid = getpid();
    pid = fork();
    if(pid < 0){
      failexit("fork");
    }
    if(pid == 0){
      printf(1, "oops could read %p = %c\n", a, *a);
      kill(ppid);
      exit();
    }
    wait();
  }

  // if we run the system out of memory, does it clean up the last
  // failed allocation?
  if(pipe(fds) != 0){
    failexit("pipe()");
  }
  printf(1, "expecting failed sbrk()s:\n");
  for(i = 0; i < sizeof(pids)/sizeof(pids[0]); i++){
    if((pids[i] = fork()) == 0){
      // allocate a lot of memory
      int ret = (int)(addr_t)sbrk(BIG - (addr_t)sbrk(0));
      if(ret < 0)
        printf(1, "sbrk returned -1 as expected\n");
      write(fds[1], "x", 1);
      // sit around until killed
      for(;;)
        sleep(1000);
    }
    if(pids[i] != -1)
      read(fds[0], &scratch, 1); // wait
  }

  // if those failed allocations freed up the pages they did allocate,
  // we'll be able to allocate one here
  c = sbrk(4096);
  for(i = 0; i < sizeof(pids)/sizeof(pids[0]); i++){
    if(pids[i] == -1)
      continue;
    kill(pids[i]);
    wait();
  }
  if(c == (char*)0xffffffff){ // ?
    failexit("failed sbrk leaked memory");
  }

  if(sbrk(0) > oldbrk)
    sbrk(-(sbrk(0) - oldbrk));

  printf(1, "sbrk test OK\n");
}

void
validatetest(void)
{
  int hi;
  addr_t p;

  printf(1, "validate test\n");
  hi = 1100*1024;

  // first page not mapped
  for(p = 4096; p <= (uint)hi; p += 4096){
    // try to crash the kernel by passing in a bad string pointer
    if(link("nosuchfile", (char*)p) != -1){
      failexit("link should not succeed.");
    }
  }

  printf(1, "validate ok\n");
}

// does unintialized data start out zero?
char uninit[10000];
void
bsstest(void)
{
  int i;

  printf(1, "bss test\n");
  for(i = 0; i < sizeof(uninit); i++){
    if(uninit[i] != '\0'){
      failexit("bss test");
    }
  }
  printf(1, "bss test ok\n");
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
    printf(1, "bigarg test\n");
    exec("echo", args);
    printf(1, "bigarg test ok\n");
    fd = open("bigarg-ok", O_CREATE);
    close(fd);
    exit();
  } else if(pid < 0){
    failexit("bigargtest: fork");
  }
  wait();
  fd = open("bigarg-ok", 0);
  if(fd < 0){
    failexit("bigarg test");
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

  printf(1, "fsfull test\n");

  for(nfiles = 0; ; nfiles++){
    char name[64];
    name[0] = 'f';
    name[1] = '0' + nfiles / 1000;
    name[2] = '0' + (nfiles % 1000) / 100;
    name[3] = '0' + (nfiles % 100) / 10;
    name[4] = '0' + (nfiles % 10);
    name[5] = '\0';
    printf(1, "writing %s\n", name);
    int fd = open(name, O_CREATE|O_RDWR);
    if(fd < 0){
      printf(1, "open %s failed\n", name);
      break;
    }
    int total = 0;
    while(1){
      int cc = write(fd, buf, 512);
      if(cc < 512)
        break;
      total += cc;
      fsblocks++;
    }
    printf(1, "wrote %d bytes\n", total);
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

  printf(1, "fsfull test finished\n");
}

void
uio()
{
  #define RTC_ADDR 0x70
  #define RTC_DATA 0x71

  ushort port = 0;
  uchar val = 0;
  int pid;

  printf(1, "uio test\n");
  pid = fork();
  if(pid == 0){
    port = RTC_ADDR;
    val = 0x09;  /* year */
    /* http://wiki.osdev.org/Inline_Assembly/Examples */
    asm volatile("outb %0,%1"::"a"(val), "d" (port));
    port = RTC_DATA;
    asm volatile("inb %1,%0" : "=a" (val) : "d" (port));
    printf(1, "uio test succeeded\n");
    exit();
  } else if(pid < 0){
    failexit("fork");
  }
  wait();
  printf(1, "uio test done\n");
}

void argptest()
{
  int fd;
  fd = open("init", O_RDONLY);
  if (fd < 0) {
    failexit("open");
  }
  read(fd, sbrk(0) - 1, -1);
  close(fd);
  printf(1, "arg test passed\n");
}

unsigned long randstate = 1;
unsigned int
rand()
{
  randstate = randstate * 1664525 + 1013904223;
  return randstate;
}

int
main(int argc, char *argv[])
{
  printf(1, "usertests starting\n");

  if(open("usertests.ran", 0) >= 0){
    failexit("already ran user tests -- rebuild fs.img");
  }
  close(open("usertests.ran", O_CREATE));

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
  nullptr();

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
  uio();

  exectest(); // will exit

  exit();
}
