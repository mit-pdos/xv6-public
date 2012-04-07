#ifdef LWIP
extern "C" {
#include "lwip/sockets.h"

// system calls
extern int socket(int domain, int type, int protocol);
extern int bind(int sockfd, const struct sockaddr *addr,
                int addrlen);
extern int listen(int sockfd, int backlog);
extern int accept(int sockfd, struct sockaddr *addr, int *addrlen);
}

static inline const char *
ipaddr(struct sockaddr_in *sin)
{
  static char buf[16];
  u32 addr = ntohl(sin->sin_addr.s_addr);

  snprintf(buf, sizeof(buf), "%d.%d.%d.%d",
           (addr & 0xff000000) >> 24,
           (addr & 0x00ff0000) >> 16,
           (addr & 0x0000ff00) >> 8,
           (addr & 0x000000ff));

  return buf;
}
#endif
