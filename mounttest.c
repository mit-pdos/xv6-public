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
#include "tester.h"


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
  int res = mount("internal_fs_a", "a", 0);
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

static int
mounttest(void) {
  if (mounta() != 0) {
    return 1;
  }
  
  if (umounta() != 0) {
    return 1;
  }

  return 0;
}

static int
statroottest(void) {
  int pid = fork();

  if (pid < 0) {
    return 1; // exit on error in fork
  }

  if (pid == 0) {
    // in child, only mount
    if (mounta() != 0) {
      return 1;
    }
    return 0;
  }

  int ret_val = child_exit_status(pid); // get child exit status
  if (ret_val != 0) {
    return 1;
  }

  struct stat st;
  stat("a", &st);
  if (st.type != T_DIR || st.ino != 1 || st.size != BSIZE) {
    return 1;
  }

  if (umounta() != 0) {
    return 1;
  }
  return 0;
}

static int
writefiletest(void) {
  if (mounta() != 0) {
    return 1;
  }

  if (testfile("a/test1") != 0) {
    return 1;
  }

  if (umounta() != 0) {
    return 1;
  }

  return 0;
}

static int
invalidpathtest(void) {
  int res = mount("internal_fs_a", "AAA", 0);
  if (res != -1) {
    printf(1, "invalidpathtest: mount did not fail as expected %d\n", res);
    return 1;
  }

  if (mounta() != 0) {
    return 1;
  }

  res = umount("b");
  if (res != -1) {
    printf(1, "invalidpathtest: umount did not fail as expected %d\n", res);
    return 1;
  }

  mkdir("b");
  res = umount("b");
  if (res != -1) {
    printf(1, "invalidpathtest: umount did not fail as expected %d\n", res);
    return 1;
  }

  if (umounta() != 0) {
    return 1;
  }

  return 0;
}

static int
doublemounttest(void) {
  if (mounta() != 0) {
    return 1;
  }

  mkdir("b");
  int res = mount("internal_fs_a", "b", 0);
  if (res != 0) {
    printf(1, "doublemounttest: mount returned %d\n", res);
    return 1;
  }

  if (umounta() != 0) {
    return 1;
  }

  res = umount("b");
  if (res != 0) {
    printf(1, "doublemounttest: umount returned %d\n", res);
    return 1;
  }

  return 0;
}

static int
samedirectorytest(void) {
  if (mounta() != 0) {
    return 1;
  }

  int res = mount("internal_fs_b", "a", 0);
  if (res != -1) {
    printf(1, "samedirectorytest: mount did not fail as expected %d\n", res);
    return 1;
  }

  if (umounta() != 0) {
    return 1;
  }

  return 0;
}

static int
directorywithintest(void) {
  if (mounta() != 0) {
    return 1;
  }

  mkdir ("a/ttt");
  if (testfile("a/ttt/test1") != 0) {
    return 1;
  }

  if (umounta() != 0) {
    return 1;
  }

  return 0;
}

static int
nestedmounttest(void) {
  if (mounta() != 0) {
    return 1;
  }

  mkdir("a/b");
  int res = mount("internal_fs_b", "a/b", 0);
  if (res != 0) {
    printf(1, "nestedmounttest: mount returned %d\n", res);
    return 1;
  }

  if (testfile("a/b/test1") != 0) {
    return 1;
  }

  res = umount("a");
  if (res != -1) {
    printf(1, "nestedmounttest: umount did not fail as expected %d\n", res);
    return 1;
  }

  res = umount("a/b");
  if (res != 0) {
    printf(1, "nestedmounttest: umount returned %d\n", res);
    return 1;
  }

  if (umounta() != 0) {
    return 1;
  }

  return 0;
}

static int
devicefilestoretest(void) {
  if (mounta() != 0) {
    return 1;
  }

  if (createfile("a/devicefilestoretest", "ababab") != 0) {
    return 1;
  }

  if (umounta() != 0) {
    return 1;
  }

  mkdir("ccc");
  int res = mount("internal_fs_a", "ccc", 0);
  if (res != 0) {
    printf(1, "devicefilestoretest: mount returned %d\n", res);
    return 1;
  }

  if (verifyfilecontents("ccc/devicefilestoretest", "ababab") != 0) {
    return 1;
  }

  res = umount("ccc");
  if (res != 0) {
    printf(1, "devicefilestoretest: umount did not fail as expected %d\n", res);
    return 1;
  }

  unlink("ccc");

  return 0;
}

static int
umountwithopenfiletest(void) {
  if (mounta() != 0) {
    return 1;
  }

  int fd;
  if((fd = open("a/umountwithop", O_WRONLY|O_CREATE)) < 0){
    printf(1, "umountwithopenfiletest: cannot open file\n");
    return 1;
  }

  int res = umount("a");
  if (res != -1) {
    printf(1, "umountwithopenfiletest: umount did not fail as expected %d\n", res);
    return 1;
  }

  close(fd);

  if (umounta() != 0) {
    return 1;
  }

  return 0;
}

static int
errorondeletedevicetest(void) {
  if (mounta() != 0) {
    return 1;
  }

  int res = unlink("internal_fs_a");
  if (res != -1) {
    printf(1, "errorondeletedevicetest: unlink did not fail as expected %d\n", res);
    return 1;
  }

  if (umounta() != 0) {
    return 1;
  }

  return 0;
}

static int
namespacetest(void) {
  if (mounta() != 0) {
    return 1;
  }

  int pid = fork();
  if (pid == 0) {
    unshare(MOUNT_NS);

    umounta();

    exit(0);
  } else {
    wait(0);
    if (umounta() != 0) {
      return 1;
    }

    return 0;
  }
}

static int
namespacefiletest(void) {
  if (mounta() != 0) {
    return 1;
  }

  int pid = fork();
  if (pid == 0) {
    unshare(MOUNT_NS);
    mkdir("b");
    int res = mount("internal_fs_b", "b", 0);
    if (res != 0) {
      printf(1, "namespacefiletest: mount returned %d\n", res);
      return 1;
    }

    createfile("b/nsfiletest", "aaa");
    exit(0);
  } else {
    wait(0);

    if (umounta() != 0) {
      return 1;
    }
    if (open("b/nsfiletest", 0) >= 0) {
      printf(1, "namespacefiletest: should not have been able to open file\n");
      return 1;
    }

    int res = mount("internal_fs_b", "b", 0);
    if (res != 0) {
      printf(1, "namespacefiletest: mount returned %d\n", res);
      return 1;
    }

    int fd;
    if ((fd = open("b/nsfiletest", 0)) < 0) {
      printf(1, "namespacefiletest: failed to open file after mount\n");
      return 1;
    }

    close(fd);

    res = umount("b");
    if (res != 0) {
      printf(1, "namespacefiletest: umount returned %d\n", res);
      return 1;
    }

    return 0;
  }
}

static int
cdinthenouttest(void) {
  if (mounta() != 0) {
    return 1;
  }

  chdir("a");
  chdir("..");

  // tmp replacment of pwd - checking if the wd contains the "a" dir
  struct stat st;
  if(stat("a", &st) < 0) {
    printf(1, "cdinthenouttest: not in root or couldnt find a dir\n");
    return 1;
  }

  int res = umount("a");
  if (res != 0) {
    printf(1, "cdinthenouttest: unmount returned %d\n", res);
    return 1;
  }
  return 0;
}



int
main(int argc, char *argv[])
{
  printf(stderr, "Running all mounttest\n");
  run_test(mounttest, "mounttest");
  run_test(statroottest, "statroottest");
  run_test(invalidpathtest, "invalidpathtest");
  run_test(doublemounttest, "doublemounttest");
  run_test(samedirectorytest, "samedirectorytest");
  run_test(writefiletest, "writefiletest");
  run_test(directorywithintest, "directorywithintest");
  run_test(nestedmounttest, "nestedmounttest");
  run_test(devicefilestoretest, "devicefilestoretest");
  run_test(umountwithopenfiletest, "umountwithopenfiletest");
  run_test(errorondeletedevicetest, "errorondeletedevicetest");
  run_test(namespacetest, "namespacetest");
  run_test(namespacefiletest, "namespacefiletest");
  run_test(cdinthenouttest, "cdinthenouttest");

  unlink("a");
  unlink("b");

  if (testsPassed == 0) {
    printf(stderr, "mounttest tests passed successfully\n");
    exit(0);
  }
  else {
    printf(stderr, "mounttest tests failed to pass\n");
    exit(1);
  }
}
