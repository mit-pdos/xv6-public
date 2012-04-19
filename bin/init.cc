// init: The initial user-level program

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "lib.h"
#include "major.h"

static const char *sh_argv[] = { "sh", 0 };
static const char *app_argv[][MAXARG] = {
#ifdef LWIP
  { "telnetd", 0 },
  { "httpd", 0 },
#endif
};

static struct {
  const char* name;
  int major;
} dev[] = {
  { "/dev/netif",     MAJ_NETIF },
  { "/dev/sampler",   MAJ_SAMPLER },
  { "/dev/lockstat",  MAJ_LOCKSTAT },
  { "/dev/stat",      MAJ_STAT },
  { "/dev/cmdline",   MAJ_CMDLINE},
};

static int
startone(const char **argv)
{
  int pid;

  pid = fork(0);
  if(pid < 0){
    fprintf(1, "init: fork failed\n");
    exit();
  }
  if(pid == 0){
    exec(argv[0], argv);
    fprintf(1, "init: exec %s failed\n", argv[0]);
    exit();
  }
  return pid;
}

static void
runcmdline(void)
{
  const char* argv[3] = { "sh", 0, 0 };
  char buf[256];
  char* b;
  long r;
  int fd;

  fd = open("/dev/cmdline", O_RDONLY);
  if (fd < 0)
    return;

  r = read(fd, buf, sizeof(buf)-1);
  if (r < 0)
    return;
  buf[r] = 0;
  
  if ((b = strchr(buf, '$'))) {
    argv[1] = b+1;
    startone(argv);
  }
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
  for (int i = 0; i < NELEM(dev); i++)
    if (mknod(dev[i].name, dev[i].major, 1) < 0)
      fprintf(2, "init: mknod %s failed\n", dev[i].name);
  
  for (u32 i = 0; i < NELEM(app_argv); i++)
    startone(app_argv[i]);

  runcmdline();

  for(;;){
    pid = startone(sh_argv);
    while((wpid=wait()) >= 0 && wpid != pid)
      fprintf(1, "zombie!\n");
  }
}
