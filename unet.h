#ifdef LWIP
#include "lwip/sockets.h"

// system calls
extern int socket(int domain, int type, int protocol);
extern int bind(int sockfd, const struct sockaddr *addr,
                socklen_t addrlen);
extern int listen(int sockfd, int backlog);
extern int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
#endif
