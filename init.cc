// init: The initial user-level program

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "lib.h"

static const char *sh_argv[] = { "sh", 0 };
static const char *app_argv[][MAXARG] = {
#ifdef LWIP
  { "telnetd", 0 },
  { "httpd", 0 },
#endif
};

static int
startone(const char **argv)
{
  int pid;

  pid = fork(0);
  if(pid < 0){
    printf(1, "init: fork failed\n");
    exit();
  }
  if(pid == 0){
    exec(argv[0], argv);
    printf(1, "init: exec %s failed\n", argv[0]);
    exit();
  }
  return pid;
}

int
main(void)
{
  int pid, wpid;

  if(open("console", O_RDWR) < 0){
    mknod("console", 1, 1);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr

  mkdir("dev");
  if (mknod("/dev/netif", 2, 1) < 0)
      printf(2, "init: mknod netif failed\n");
  if (mknod("/dev/sampler", 3, 1) < 0)
      printf(2, "init: mknod sampler failed\n");
  if (mknod("/dev/lockstat", 4, 1) < 0)
      printf(2, "init: mknod lockstat failed\n");

  for (u32 i = 0; i < NELEM(app_argv); i++)
    startone(app_argv[i]);

  for(;;){
    pid = startone(sh_argv);
    while((wpid=wait()) >= 0 && wpid != pid)
      printf(1, "zombie!\n");
  }
}
