#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char buf[512];

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
    printf(1, "fstests: cannot open sharedfd for writing");
    return;
  }
  pid = fork();
  memset(buf, pid==0?'c':'p', sizeof(buf));
  for(i = 0; i < 100; i++){
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

  unlink("f1");
  unlink("f2");

  puts("twofiles ok\n");
}

// two processes create and delete files in same directory
void
createdelete()
{
  int pid, i, fd;
  int n = 20;
  char name[32];

  pid = fork();
  if(pid < 0){
    puts("fork failed\n");
    exit();
  }

  name[0] = pid ? 'p' : 'c';
  name[2] = '\0';
  for(i = 0; i < n; i++){
    name[1] = '0' + i;
    fd = open(name, O_CREATE | O_RDWR);
    if(fd < 0){
      puts("create failed\n");
      exit();
    }
    close(fd);
    if(i > 0 && (i % 2 ) == 0){
      name[1] = '0' + (i / 2);
      if(unlink(name) < 0){
        puts("unlink failed\n");
        exit();
      }
    }
  }

  if(pid)
    wait();
  else
    exit();

  for(i = 0; i < n; i++){
    name[0] = 'p';
    name[1] = '0' + i;
    fd = open(name, 0);
    if((i == 0 || i >= n/2) && fd < 0){
      printf(1, "oops createdelete %s didn't exist\n", name);
    } else if((i >= 1 && i < n/2) && fd >= 0){
      printf(1, "oops createdelete %s did exist\n", name);
    }
    if(fd >= 0)
      close(fd);

    name[0] = 'c';
    name[1] = '0' + i;
    fd = open(name, 0);
    if((i == 0 || i >= n/2) && fd < 0){
      printf(1, "oops createdelete %s didn't exist\n", name);
    } else if((i >= 1 && i < n/2) && fd >= 0){
      printf(1, "oops createdelete %s did exist\n", name);
    }
    if(fd >= 0)
      close(fd);
  }

  for(i = 0; i < n; i++){
    name[0] = 'p';
    name[1] = '0' + i;
    unlink(name);
    name[0] = 'c';
    unlink(name);
  }

  printf(1, "createdelete ok\n");
}

// can I unlink a file and still read it?
void
unlinkread()
{
  int fd, fd1;
  
  fd = open("unlinkread", O_CREATE | O_RDWR);
  if(fd < 0){
    puts("create unlinkread failed\n");
    exit();
  }
  write(fd, "hello", 5);
  close(fd);

  fd = open("unlinkread", O_RDWR);
  if(fd < 0){
    puts("open unlinkread failed\n");
    exit();
  }
  if(unlink("unlinkread") != 0){
    puts("unlink unlinkread failed\n");
    exit();
  }

  fd1 = open("xxx", O_CREATE | O_RDWR);
  write(fd1, "yyy", 3);
  close(fd1);

  if(read(fd, buf, sizeof(buf)) != 5){
    puts("unlinkread read failed");
    exit();
  }
  if(buf[0] != 'h'){
    puts("unlinkread wrong data\n");
    exit();
  }
  if(write(fd, buf, 10) != 10){
    puts("unlinkread write failed\n");
    exit();
  }
  close(fd);
  unlink("xxx");
  puts("unlinkread ok\n");
}

void
linktest()
{
  int fd;

  unlink("lf1");
  unlink("lf2");

  fd = open("lf1", O_CREATE|O_RDWR);
  if(fd < 0){
    puts("create lf1 failed\n");
    exit();
  }
  if(write(fd, "hello", 5) != 5){
    puts("write lf1 failed\n");
    exit();
  }
  close(fd);

  if(link("lf1", "lf2") < 0){
    puts("link lf1 lf2 failed\n");
    exit();
  }
  unlink("lf1");

  if(open("lf1", 0) >= 0){
    puts("unlinked lf1 but it is still there!\n");
    exit();
  }

  fd = open("lf2", 0);
  if(fd < 0){
    puts("open lf2 failed\n");
    exit();
  }
  if(read(fd, buf, sizeof(buf)) != 5){
    puts("read lf2 failed\n");
    exit();
  }
  close(fd);
    
  if(link("lf2", "lf2") >= 0){
    puts("link lf2 lf2 succeeded! oops\n");
    exit();
  }

  unlink("lf2");
  if(link("lf2", "lf1") >= 0){
    puts("link non-existant succeeded! oops\n");
    exit();
  }

  if(link(".", "lf1") >= 0){
    puts("link . lf1 succeeded! oops\n");
    exit();
  }

  puts("linktest ok\n");
}

// test concurrent create of the same file
void
concreate()
{
  char file[3];
  int i, pid, n, fd;
  char fa[40];
  struct {
    unsigned short inum;
    char name[14];
  } de;

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
        puts("concreate create failed\n");
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
    puts("concreate not enough files in directory listing\n");
    exit();
  }

  for(i = 0; i < 40; i++){
    file[1] = '0' + i;
    unlink(file);
  }

  puts("concreate ok\n");
}

int
main(int argc, char *argv[])
{
  puts("fstests starting\n");

  concreate();
  linktest();
  unlinkread();
  createdelete();
  twofiles();
  sharedfd();

  puts("fstests finished\n");
  exit();
}
