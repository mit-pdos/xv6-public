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

  for (;;) {
    socklen_t socklen = sizeof(sin);
    int ss = accept(s, (struct sockaddr *)&sin, &socklen);
    if (ss < 0) {
      printf(1, "telnetd accept: %d\n", ss);
      continue;
    }
    
    printf(1, "Got one!\n");
    // XXX fork, exec sh, etc..
    close(ss);
  }
}
