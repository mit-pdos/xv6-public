#define _GNU_SOURCE
#include "syscall.h"
#include "types.h"
#include "user.h"
#include "wstatus.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define CLONE_NEWPID 2
#define NULL 0

typedef   signed int          pid_t;
typedef   signed int          size_t;

int stderr = 2;

int check(int r, const char *msg) {
  if (r < 0) {
    printf(stderr, "%s\n", (char *)msg);
    exit(1);
  }

  return r;
}

void assert_msg(int r, const char *msg) {
  if (r) {
    return;
  }
  printf(stderr, "%s\n", (char *)msg);
  exit(1);
}

static int child_exit_status(int pid) {
  int changed_pid = -1;
  int wstatus;
  do {
    changed_pid = check(wait(&wstatus), "failed to waitpid");
  } while (changed_pid != pid);

  // TODO: there is no exit status in xv6
  return WEXITSTATUS(wstatus);
}

static void write_all(int fd, unsigned char *buf, size_t len) {
  size_t bytes_read = 0;

  while (bytes_read < len) {
    size_t ret =
        check(write(fd, &buf[bytes_read], len - bytes_read), "failed to write");
    bytes_read += ret;
  }
}

static void read_all(int fd, unsigned char *buf, size_t len) {
  size_t bytes_read = 0;

  while (bytes_read < len) {
    size_t ret =
        check(read(fd, &buf[bytes_read], len - bytes_read), "failed to read");
    bytes_read += ret;
  }
}

/* static void wait_for(int fd, char *s) { */
/*   char buf[0x80]; */
/*   size_t len = strlen(s); */

/*   if (len + 1 >= sizeof(buf)) { */
/*     exit(1); */
/*   } */
/*   int ret = check(read(fd, buf, len + 1), "read"); */
/*   if (ret != len + 1) { */
/*     exit(1); */
/*   } */
/*   if (strcmp(s, buf) != 0) */
/*     exit(1); */
/* } */

typedef int (*test_func_t)(void);

int test_simple_pidns_fork() {
  check(unshare(CLONE_NEWPID), "failed to unshare");

  int ret = check(fork(), "failed to fork");
  // child
  if (ret == 0) {
    assert_msg(getpid() == 1, "pid not equal to 1");

    ret = check(fork(), "failed to fork 2");
    // child
    if (ret == 0) {
      assert_msg(getpid() == 2, "pid not equal to 2");
      exit(0);
    }

    int status = child_exit_status(ret);
    assert_msg(status == 0, "child process failed");
    exit(0);
  }

  // flaky test because pid can recycle. However strictly speaking pid should be
  // increasing
  assert_msg(getpid() < ret, "wrong pid");

  int status = child_exit_status(ret);
  assert_msg(status == 0, "child process failed");
  return 0;
}

int test_simple_pidns() {
  check(unshare(CLONE_NEWPID), "failed to unshare");

  int ret = check(fork(), "failed to fork");
  // child
  if (ret == 0) {
    printf(1, "getpid = %d\n", getpid());
    assert_msg(getpid() == 1, "pid not equal to 1");
    exit(0);
  }

  // flaky test because pid can recycle. However strictly speaking pid should be
  // increasing
  printf(1, "getpid = %d, ret = %d\n", getpid(), ret);
  assert_msg(getpid() < ret, "wrong pid");

  int status = child_exit_status(ret);
  assert_msg(status == 0, "child process failed");
  return 0;
}

int test_nested_pidns_create() {
  check(unshare(CLONE_NEWPID), "failed to unshare");

  int ret = check(fork(), "failed to fork");
  if (ret == 0) {
    assert_msg(getpid() == 1, "pid not equal to 1");
    exit(test_simple_pidns_fork());
  }

  int status = child_exit_status(ret);
  assert_msg(status == 0, "child process failed");
  return 0;
}

void create_children(int n, pid_t *child_pids, test_func_t func) {
  for (int i = 0; i < n; i++) {
    int ret = check(fork(), "failed to fork in create_children");
    if (ret == 0) {
      // child
      exit(func());
    }
    if (child_pids) {
      child_pids[i] = ret;
    }
  }
}

void reap_children(int n, pid_t *child_pids) {
  int count = 0;
  int wstatus = 0;
  while (count < n) {
    int pid = check(wait(&wstatus), "failed to wait()");

    // if we don't verify the child pids just count them
    if (!child_pids) {
      count++;
      continue;
    }

    for (int i = 0; i < n; i++) {
      if (child_pids[i] == pid) {
        count++;
        assert_msg(WEXITSTATUS(wstatus) == 0, "exit code is not 0");
        break;
      }
    }
  }
}

int loop_forever() {
  while (1) {
  }
  return 0;
}

int sleep_1s() {
  // TODO: find a better way to sync the destruction
  sleep(1);
  return 0;
}

int test_children_reaped_by_nspid1() {
  check(unshare(CLONE_NEWPID), "failed to unshare");
  int ret = check(fork(), "failed to fork");
  if (ret == 0) {
    pid_t child_pids[2];
    int fd[2];

    check(pipe(fd), "failed to create pipes");

    // child is pid 1
    assert_msg(getpid() == 1, "pid not equal to 1");

    // create pid 2
    int ret = check(fork(), "failed to fork2");
    if (ret == 0) {
      create_children(ARRAY_SIZE(child_pids), child_pids, sleep_1s);
      write_all(fd[1], (void *)child_pids, sizeof(child_pids));

      // cleanup fds
      close(fd[0]);
      close(fd[1]);

      // kill pid 2
      exit(0);
    }

    read_all(fd[0], (void *)child_pids, sizeof(child_pids));

    // cleanup fds
    close(fd[0]);
    close(fd[1]);

    // reap child pids
    reap_children(ARRAY_SIZE(child_pids), child_pids);
    exit(0);
  }

  int status = child_exit_status(ret);
  assert_msg(status == 0, "child process failed");
  return 0;
}

int test_all_children_kill_when_nspid1_dies() {
  check(unshare(CLONE_NEWPID), "failed to unshare");

  int ret = check(fork(), "failed to fork");
  if (ret == 0) {

    // child is pid 1
    assert_msg(getpid() == 1, "pid not equal to 1");

    create_children(2, NULL, loop_forever);

    // pid 1 exits
    exit(0);
  }

  // Note: I'm supposed to only wait on one process sincew the other proccesses
  // are reaped by the kernel (which is inconsistent and wierd).

  // This test will fail on an infinite loop
  int status = child_exit_status(ret);
  assert_msg(status == 0, "child process failed");
  return 0;
}

int test_calling_fork_after_nspid1_dies_fails() {
  check(unshare(CLONE_NEWPID), "failed to unshare");

  int ret = check(fork(), "failed to fork");
  if (ret == 0) {

    // child is pid 1
    assert_msg(getpid() == 1, "pid not equal to 1");

    // pid 1 exits
    exit(0);
  }

  // make sure it's dead
  int status = child_exit_status(ret);
  assert_msg(status == 0, "child process failed");

  ret = fork();
  assert_msg(ret < 0, "fork didn't fail as expected");

  return 0;
}

int MAX_RECURSION = 32;

int _test_unshare_recrusive_limit(int count) {
  if (count == 0) {
    assert_msg(unshare(CLONE_NEWPID) < 0, "unshare didn't fail as expected");
    return 0;
  }

  check(unshare(CLONE_NEWPID), "failed to unshare");

  int ret = check(fork(), "failed to fork");
  if (ret == 0) {

    // child is pid 1
    assert_msg(getpid() == 1, "pid not equal to 1");

    _test_unshare_recrusive_limit(count - 1);

    // pid 1 exits
    exit(0);
  }

  // make sure it's dead
  int status = child_exit_status(ret);
  assert_msg(status == 0, "child process failed");

  return 0;
}

int test_unshare_recrusive_limit() { 
  _test_unshare_recrusive_limit(MAX_RECURSION);
  return 0; 
}

int unshare_twice() {
  // first call should succeed
  int ret = unshare(CLONE_NEWPID);
  if (ret < 0) {
    return 1;
  }

  // second call should fail
  ret = unshare(CLONE_NEWPID);
  if (ret < 0) {
    return 0;
  }

  return 1;
}

int run_test(test_func_t func, const char *name) {
  int status = 0;
  int pid = -1;

  printf(stderr, "running test - '%s'\n", name);
  int ret = check(fork(), "fork failed");
  if (ret == 0) {
    exit(func());
  }

  pid = ret;
  if (child_exit_status(pid) != 0) {
    printf(stderr, "failed test - '%s'\n", name);
  }

  return status;
}

int main() {
  run_test(unshare_twice, "unshare_twice");
  run_test(test_simple_pidns, "test_simple_pidns");
  run_test(test_simple_pidns_fork, "test_simple_pidns_fork");
  /* run_test(test_nested_pidns_create, "test_nested_pidns_create"); */
  /* run_test(test_children_reaped_by_nspid1, "test_children_reaped_by_nspid1"); */
  /* run_test(test_all_children_kill_when_nspid1_dies, */
  /*          "test_all_children_kill_when_nspid1_dies"); */
  /* run_test(test_calling_fork_after_nspid1_dies_fails, "test_calling_fork_after_nspid1_dies_fails"); */
  // run_test(test_calling_fork_recursive_after_nspid1_dies_fails, "test_calling_fork_recursive_after_nspid1_dies_fails");
 
  /* run_test(test_unshare_recrusive_limit, "test_unshare_recrusive_limit"); */

  exit(0);
}
