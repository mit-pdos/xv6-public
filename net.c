#ifdef LWIP
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#include "lwip/tcp_impl.h"
#include "lwip/tcpip.h"
#include "lwip/ip.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "lwip/sockets.h"
#include "netif/etharp.h"
#pragma GCC diagnostic pop
#endif

#include "types.h"
#include "kernel.h"
#include "queue.h"
#ifndef LWIP
#include "spinlock.h"
#include "condvar.h"
#endif
#include "proc.h"
#include "fs.h"
#include "file.h"

void
netfree(void *va)
{
  kfree(va);
}

void *
netalloc(void)
{
  return kalloc();
}

int
nettx(void *va, u16 len)
{
  return e1000tx(va, len);
}

void
nethwaddr(u8 *hwaddr)
{
  e1000hwaddr(hwaddr);
}

#ifdef LWIP

static struct netif nif;

struct timer_thread {
  u64 nsec;
  struct condvar waitcv;
  struct spinlock waitlk;
  void (*func)(void);
};

int errno;

void
netrx(void *va, u16 len)
{
  extern void if_input(struct netif *netif, void *buf, u16 len);

  lwip_core_lock();
  if_input(&nif, va, len);
  lwip_core_unlock();
}

static void __attribute__((noreturn))
net_timer(void *x)
{
    struct timer_thread *t = (struct timer_thread *) x;

    for (;;) {
      u64 cur = nsectime();

      lwip_core_lock();
      t->func();
      lwip_core_unlock();
      acquire(&t->waitlk);
      cv_sleepto(&t->waitcv, &t->waitlk, cur + t->nsec);
      release(&t->waitlk);
    }
}

static void
start_timer(struct timer_thread *t, void (*func)(void),
            const char *name, u64 msec)
{
  struct proc *p;

  t->nsec = 1000000000 / 1000*msec;
  t->func = func;
  initcondvar(&t->waitcv, name);
  initlock(&t->waitlk, name);
  p = threadalloc(net_timer, t);
  if (p == NULL)
    panic("net: start_timer");

  acquire(&p->lock);
  safestrcpy(p->name, name, sizeof(p->name));
  p->state = RUNNABLE;
  addrun(p);
  release(&p->lock);
}

static void
lwip_init(struct netif *nif, void *if_state,
	  u32 init_addr, u32 init_mask, u32 init_gw)
{
  extern err_t if_init(struct netif *netif);

  struct ip_addr ipaddr, netmask, gateway;
  ipaddr.addr  = init_addr;
  netmask.addr = init_mask;
  gateway.addr = init_gw;
  
  if (0 == netif_add(nif, &ipaddr, &netmask, &gateway,
                     if_state,
                     if_init,
                     ip_input))
    panic("lwip_init: error in netif_add\n");
  
  netif_set_default(nif);
  netif_set_up(nif);
}

static void
tcpip_init_done(void *arg)
{
  volatile long *tcpip_done = arg;
  *tcpip_done = 1;
}

void
initnet_worker(void *x)
{
  static struct timer_thread t_arp, t_tcpf, t_tcps, t_dhcpf, t_dhcpc;
  volatile long tcpip_done = 0;

  lwip_core_init();

  lwip_core_lock();
  tcpip_init(&tcpip_init_done, (void*)&tcpip_done);
  lwip_core_unlock();
  while (!tcpip_done)
    yield();

  lwip_core_lock();
  memset(&nif, 0, sizeof(nif));
  lwip_init(&nif, NULL, 0, 0, 0);

  dhcp_start(&nif);

  start_timer(&t_arp, &etharp_tmr, "arp timer", ARP_TMR_INTERVAL);
  start_timer(&t_tcpf, &tcp_fasttmr, "tcp f timer", TCP_FAST_INTERVAL);
  start_timer(&t_tcps, &tcp_slowtmr, "tcp s timer", TCP_SLOW_INTERVAL);

  start_timer(&t_dhcpf, &dhcp_fine_tmr,	"dhcp f timer",	DHCP_FINE_TIMER_MSECS);
  start_timer(&t_dhcpc, &dhcp_coarse_tmr, "dhcp c timer", DHCP_COARSE_TIMER_MSECS);

  struct spinlock lk;
  struct condvar cv;
  int dhcp_state = 0;
  const char *dhcp_states[] = {
    [DHCP_RENEWING]  = "renewing",
    [DHCP_SELECTING] = "selecting",
    [DHCP_CHECKING]  = "checking",
    [DHCP_BOUND]     = "bound",
  };

  initlock(&lk, "dhcp sleep");
  initcondvar(&cv, "dhcp cv sleep");

  for (;;) {
    if (dhcp_state != nif.dhcp->state) {
      dhcp_state = nif.dhcp->state;
      cprintf("net: DHCP state %d (%s)\n", dhcp_state,
              dhcp_states[dhcp_state] ? : "unknown");

      if (dhcp_state == DHCP_BOUND) {
        u32 ip = ntohl(nif.ip_addr.addr);
        cprintf("net: %02x:%02x:%02x:%02x:%02x:%02x" 
                " bound to %u.%u.%u.%u\n", 
                nif.hwaddr[0], nif.hwaddr[1], nif.hwaddr[2],
                nif.hwaddr[3], nif.hwaddr[4], nif.hwaddr[5],
                (ip & 0xff000000) >> 24,
                (ip & 0x00ff0000) >> 16,
                (ip & 0x0000ff00) >> 8,
                (ip & 0x000000ff));
      }
    }

    lwip_core_unlock();    
    acquire(&lk);
    cv_sleepto(&cv, &lk, nsectime() + 1000000000);
    release(&lk);
    lwip_core_lock();
  }
}

void
initnet(void)
{
  struct proc *t;

  t = threadalloc(initnet_worker, NULL);
  if (t == NULL)
    panic("initnet: threadalloc");

  acquire(&t->lock);
  t->state = RUNNABLE;
  addrun(t);
  release(&t->lock);
}

long
netsocket(int domain, int type, int protocol)
{
  int r;
  lwip_core_lock();
  r = lwip_socket(domain, type, protocol);
  lwip_core_unlock();
  return r;
}

long
netbind(int sock, void *xaddr, int xaddrlen)
{
  void *addr;
  long r;
  
  addr = kmalloc(xaddrlen);
  if (addr == NULL)
    return -1;

  if (umemcpy(addr, xaddr, xaddrlen))
    return -1;

  lwip_core_lock();
  r = lwip_bind(sock, addr, xaddrlen);
  lwip_core_unlock();
  kmfree(addr);
  return r;
}

long
netlisten(int sock, int backlog)
{
  int r;
  
  lwip_core_lock();
  r = lwip_listen(sock, backlog);
  lwip_core_unlock();
  return r;
}

long
netaccept(int sock, void *xaddr, void *xaddrlen)
{
  socklen_t *lenptr = xaddrlen;
  socklen_t len;
  void *addr;
  int ss;

  if (umemcpy(&len, lenptr, sizeof(*lenptr)))
    return -1;

  addr = kmalloc(len);
  if (addr == NULL)
    return -1;

  lwip_core_lock();
  ss = lwip_accept(sock, addr, &len);
  lwip_core_unlock();
  if (ss < 0) {
    kmfree(addr);
    return ss;
  }

  if (kmemcpy(xaddrlen, &len, sizeof(len)) || kmemcpy(xaddr, addr, len)) {
    lwip_core_lock();
    lwip_close(ss);
    lwip_core_unlock();
    kmfree(addr);
    return -1;
  }

  return ss;
}

void
netclose(int sock)
{
  lwip_core_lock();
  lwip_close(sock);
  lwip_core_unlock();
}

int
netwrite(int sock, char *ubuf, int len)
{
  void *kbuf;
  int cc;
  int r;

  kbuf = kalloc();
  if (kbuf == NULL)
    return -1;

  cc = MIN(len, PGSIZE);
  if (umemcpy(kbuf, ubuf, cc)) {
    kfree(kbuf);
    return -1;
  }
  lwip_core_lock();
  r = lwip_write(sock, kbuf, cc);
  lwip_core_unlock();
  kfree(kbuf);
  return r;
}

int
netread(int sock, char *ubuf, int len)
{
  void *kbuf;
  int cc;
  int r;

  kbuf = kalloc();
  if (kbuf == NULL)
    return -1;

  cc = MIN(len, PGSIZE);
  lwip_core_lock();
  r = lwip_read(sock, kbuf, cc);
  lwip_core_unlock();
  if (r < 0) {
    kfree(kbuf);
    return r;
  }

  kmemcpy(ubuf, kbuf, r);
  kfree(kbuf);
  return r;
}

#else

void
initnet(void)
{
}

void
netrx(void *va, u16 len)
{
  cprintf("netrx: %u\n", len);
}

long
netsocket(int domain, int type, int protocol)
{
  return -1;
}

long
netbind(int sock, void *xaddr, int xaddrlen)
{
  return -1;
}

long
netlisten(int sock, int backlog)
{
  return -1;
}

long
netaccept(int sock, void *xaddr, void *xaddrlen)
{
  return -1;
}

void
netclose(int sock)
{
}

int
netwrite(int sock, char *buf, int len)
{
  return -1;
}

int
netread(int sock, const char *buf, int len)
{
  return -1;
}
#endif
