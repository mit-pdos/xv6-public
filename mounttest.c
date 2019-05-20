#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"
#include "ns_types.h"

static void
fstat_file(char *path, struct stat *st) {
  int fd;
  if((fd = open(path, 0)) < 0){
    printf(1, "mounttest: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, st) < 0){
    printf(1, "mounttest: cannot stat %s\n", path);
    close(fd);
    return;
  }

  close(fd);
}

static int
createfile(char *path, char *contents) {
  int fd;
  if((fd = open(path, O_WRONLY|O_CREATE)) < 0){
    printf(1, "createfile: cannot open %s\n", path);
    return -1;
  }

  if (write(fd, contents, strlen(contents)) < 0) {
    printf(1, "createfile: failed writing\n", path);
    close(fd);
    return -1;
  }

  close(fd);

  return 0;
}

static int
verifyfilecontents(char *path, char *contents) {
  int fd;
  struct stat st;
  if((fd = open(path, 0)) < 0){
    printf(1, "verifyfilecontents: cannot open %s\n", path);
    return -1;
  }

  if(fstat(fd, &st) < 0){
    printf(1, "verifyfilecontents: cannot stat %s\n", path);
    close(fd);
    return -1;
  }

  int contentlen = strlen(contents);


  if (st.size != contentlen) {
    printf(1, "verifyfilecontents: incorrect length (%d) for file %s\n", st.size, path);
    close(fd);
    return -1;
  }

  char buf[100];
  int res;
  if ((res = read(fd, buf, contentlen)) != contentlen) {
    printf(1, "verifyfilecontents: incorrect length read (%d) for file %s\n", res, path);
    close(fd);
    return -1;
  }
  buf[contentlen] = '\0';
  close(fd);

  if ((res = strcmp(contents, buf)) != 0) {
    printf(1, "verifyfilecontents: incorrect content read (%s) for file %s\n", buf, path); 
    return -1;
  }

  return 0;
}

static int
testfile(char *path) {
  if (createfile(path, "aaa") != 0) {
    return -1;
  }

  if (verifyfilecontents(path, "aaa") != 0) {
    return -1;
  }

  return 0;
}

static int
mounta(void) {
  mkdir("a");
  int res = mount("internal_fs_a", "a");
  if (res != 0) {
    printf(1, "mounta: mount returned %d\n", res);
    return -1;
  }

  return 0;
}

static int
umounta(void) {
  int res = umount("a");
  if (res != 0) {
    printf(1, "umounta: umount returned %d\n", res);
    return -1;
  }

  return 0;
}

static void
mounttest(void) {
  if (mounta() != 0) {
    return;
  }
  
  if (umounta() != 0) {
    return;
  }

  printf(1, "mounttest: SUCCESS\n");
}

static void
statroottest(void) {
  if (mounta() != 0) {
    return;
  }

  struct stat st;
  fstat_file("a", &st);
  if (st.type != T_DIR || st.ino != 1 || st.size != BSIZE) {
    printf(1, "statroottest: FAILED - %d %d %d\n", st.type, st.ino, st.size);
    return;
  }

  if (umounta() != 0) {
    return;
  }

  printf(1, "statroottest: SUCCESS\n");
}

static void
writefiletest(void) {
  if (mounta() != 0) {
    return;
  }

  if (testfile("a/test1") != 0) {
    return;
  }

  if (umounta() != 0) {
    return;
  }

  printf(1, "writefiletest: SUCCESS\n"); 
}

static void
invalidpathtest(void) {
  int res = mount("internal_fs_a", "AAA");
  if (res != -1) {
    printf(1, "invalidpathtest: mount did not fail as expected %d\n", res);
    return;
  }

  if (mounta() != 0) {
    return;
  }

  res = umount("b");
  if (res != -1) {
    printf(1, "invalidpathtest: umount did not fail as expected %d\n", res);
    return;
  }

  mkdir("b");
  res = umount("b");
  if (res != -1) {
    printf(1, "invalidpathtest: umount did not fail as expected %d\n", res);
    return;
  }

  if (umounta() != 0) {
    return;
  }

  printf(1, "invalidpathtest: SUCCESS\n");
}

static void
doublemounttest(void) {
  if (mounta() != 0) {
    return;
  }

  mkdir("b");
  int res = mount("internal_fs_a", "b");
  if (res != 0) {
    printf(1, "doublemounttest: mount returned %d\n", res);
    return;
  }

  if (umounta() != 0) {
    return;
  }

  res = umount("b");
  if (res != 0) {
    printf(1, "doublemounttest: umount returned %d\n", res);
    return;
  }

  printf(1, "doublemounttest: SUCCESS\n");
}

static void
samedirectorytest(void) {
  if (mounta() != 0) {
    return;
  }

  int res = mount("internal_fs_b", "a");
  if (res != -1) {
    printf(1, "samedirectorytest: mount did not fail as expected %d\n", res);
    return;
  }

  if (umounta() != 0) {
    return;
  }

  printf(1, "samedirectorytest: SUCCESS\n");
}

static void
directorywithintest(void) {
  if (mounta() != 0) {
    return;
  }

  mkdir ("a/ttt");
  if (testfile("a/ttt/test1") != 0) {
    return;
  }

  if (umounta() != 0) {
    return;
  }

  printf(1, "directorywithintest: SUCCESS\n");
}

static void
nestedmounttest(void) {
  if (mounta() != 0) {
    return;
  }

  mkdir("a/b");
  int res = mount("internal_fs_b", "a/b");
  if (res != 0) {
    printf(1, "nestedmounttest: mount returned %d\n", res);
    return;
  }

  if (testfile("a/b/test1") != 0) {
    return;
  }

  res = umount("a");
  if (res != -1) {
    printf(1, "nestedmounttest: umount did not fail as expected %d\n", res);
    return;
  }

  res = umount("a/b");
  if (res != 0) {
    printf(1, "nestedmounttest: umount returned %d\n", res);
    return;
  }

  if (umounta() != 0) {
    return;
  }

  printf(1, "nestedmounttest: SUCCESS\n");
}

static void
devicefilestoretest(void) {
  if (mounta() != 0) {
    return;
  }

  if (createfile("a/devicefilestoretest", "ababab") != 0) {
    return;
  }
  
  if (umounta() != 0) {
    return;
  }

  mkdir("ccc");
  int res = mount("internal_fs_a", "ccc");
  if (res != 0) {
    printf(1, "devicefilestoretest: mount returned %d\n", res);
    return;
  }

  if (verifyfilecontents("ccc/devicefilestoretest", "ababab") != 0) {
    return;
  }

  res = umount("ccc");
  if (res != 0) {
    printf(1, "devicefilestoretest: umount did not fail as expected %d\n", res);
    return;
  }

  unlink("ccc");

  printf(1, "devicefilestoretest: SUCCESS\n");
}

static void
umountwithopenfiletest(void) {
  if (mounta() != 0) {
    return;
  }

  int fd;
  if((fd = open("a/umountwithop", O_WRONLY|O_CREATE)) < 0){
    printf(1, "umountwithopenfiletest: cannot open file\n");
    return;
  }

  int res = umount("a");
  if (res != -1) {
    printf(1, "umountwithopenfiletest: umount did not fail as expected %d\n", res);
    return;
  }

  close(fd);

  if (umounta() != 0) {
    return;
  }

  printf(1, "umountwithopenfiletest: SUCCESS\n");
}

static void
printheader(char *s) {
  printf(1, "----------------------------\n");
  printf(1, "--- %s\n", s);
  printf(1, "----------------------------\n");
}

static void
errorondeletedevicetest(void) {
  if (mounta() != 0) {
    return;
  }

  int res = unlink("internal_fs_a");
  if (res != -1) {
    printf(1, "errorondeletedevicetest: unlink did not fail as expected %d\n", res);
    return;
  }

  if (umounta() != 0) {
    return;
  }

  printf(1, "errorondeletedevicetest: SUCCESS\n");
}

static void
namespacetest(void) {
  if (mounta() != 0) {
    return;
  }

  int pid = fork();
  if (pid == 0) {
    unshare(MOUNT_NS);

    umounta();

    exit(0);
  } else {
    wait(0);
    if (umounta() != 0) {
      return;
    }
    
    printf(1, "namespacetest: SUCCESS\n");
  }
}

static void
namespacefiletest(void) {
  if (mounta() != 0) {
    return;
  }
  int pid = fork();
  if (pid == 0) {
    unshare(MOUNT_NS);
    mkdir("b");
    int res = mount("internal_fs_b", "b");
    if (res != 0) {
      printf(1, "namespacefiletest: mount returned %d\n", res);
      return;
    }

    createfile("b/nsfiletest", "aaa");
    exit(0);
  } else {
    wait(0);

    if (umounta() != 0) {
      return;
    }
    if (open("b/nsfiletest", 0) >= 0) {
      printf(1, "namespacefiletest: should not have been able to open file\n");
      return;
    }

    int res = mount("internal_fs_b", "b");
    if (res != 0) {
      printf(1, "namespacefiletest: mount returned %d\n", res);
      return;
    }

    int fd;
    if ((fd = open("b/nsfiletest", 0)) < 0) {
      printf(1, "namespacefiletest: failed to open file after mount\n");
      return;
    }

    close(fd);

    res = umount("b");
    if (res != 0) {
      printf(1, "namespacefiletest: umount returned %d\n", res);
      return;
    }
    
    printf(1, "namespacefiletest: SUCCESS\n");
  }
}

int
main(int argc, char *argv[])
{
  printheader("Basic unit tests:");
  mounttest();
  statroottest();
  invalidpathtest();
  doublemounttest();
  samedirectorytest();

  printheader("Scenario tests:");
  writefiletest();
  directorywithintest();
  nestedmounttest();
  devicefilestoretest();
  umountwithopenfiletest();
  errorondeletedevicetest();

  printheader("Mount namespace tests:");
  namespacetest();
  namespacefiletest();

  printheader("Cleaning up:");
  unlink("a");
  unlink("b");

  exit(0);
}
