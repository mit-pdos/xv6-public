#include "types.h"
#include "user.h"
#include "unet.h"

int
main(void)
{
  int s;
  int r;

  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0)
    die("telnetd socket: %d\n", s);

  struct sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_port = htons(23);
  r = bind(s, (struct sockaddr *)&sin, sizeof(sin));
  if (r < 0)
    die("telnetd bind: %d\n", r);
  
  r = listen(s, 5);
  if (r < 0)
    die("telnetd listen: %d\n", r);

  printf(1, "telnetd: port 23\n");

  for (;;) {
    socklen_t socklen;
    int ss;
    int pid;

    socklen = sizeof(sin);
    ss = accept(s, (struct sockaddr *)&sin, &socklen);
    if (ss < 0) {
      printf(2, "telnetd accept: %d\n", ss);
      continue;
    }
    printf(1, "telnetd: connection %s\n", ipaddr(&sin));

    pid = fork(0);
    if (pid < 0) {
      printf(2, "telnetd fork: %d\n", pid);
      close(ss);
      continue;
    }
    if (pid == 0) {
      static char *argv[] = { "sh", 0 };
      close(0);
      close(1);
      close(2);
      dup(ss);
      dup(ss);
      dup(ss);
      exec("sh", argv);
      exit();
    }
    close(ss);
  }
}
