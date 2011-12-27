#ifdef LWIP
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#include "lwip/tcp_impl.h"
#include "lwip/tcpip.h"
#include "lwip/ip.h"
#include "lwip/netif.h"
#include "netif/etharp.h"
#pragma GCC diagnostic pop
#endif

#include "types.h"
#include "kernel.h"
#include "queue.h"
#include "proc.h"

static u8 ping_packet[] = {
  0x52, 0x55, 0x0a, 0x00, 0x02, 0x02, 0x52, 0x54, 0x00, 0x12,
  0x34, 0x56, 0x08, 0x00, 0x45, 0x00, 0x00, 0x54, 0x00, 0x00,
  0x40, 0x00, 0x40, 0x01, 0x22, 0x99, 0x0a, 0x00, 0x02, 0x0f,
  0x0a, 0x00, 0x02, 0x02, 0x08, 0x00, 0x94, 0xa0, 0x03, 0x07,
  0x00, 0x01, 0x71, 0xfc, 0xec, 0x4e, 0x00, 0x00, 0x00, 0x00,
  0x3b, 0x39, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11,
  0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
  0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
  0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
};

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

void
netrx(void *va, u16 len)
{
  cprintf("netrx %lx len %x\n", va, len);
}

void
nettest(void)
{
  void *ping;
  u32 len;

  ping = netalloc();
  len = sizeof(ping_packet);
  memmove(ping, ping_packet, len);
  e1000tx(ping, len);
  
  //e1000tx(ping_packet, sizeof(ping_packet));
}

#ifdef LWIP

struct timer_thread {
  u64 nsec;
  struct condvar waitcv;
  struct spinlock waitlk;
  void (*func)(void);
};

static struct timer_thread t_arp;
static struct timer_thread t_tcpf;
static struct timer_thread t_tcps;

int errno;

static void __attribute__((noreturn))
net_timer(void *x)
{
    struct timer_thread *t = (struct timer_thread *) x;

    for (;;) {
      u64 cur = nsectime();

      t->func();
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
  static struct netif nif;
  volatile long tcpip_done = 0;

  tcpip_init(&tcpip_init_done, (void*)&tcpip_done);
  while (!tcpip_done)
    yield();

  memset(&nif, 0, sizeof(nif));
  lwip_init(&nif, NULL, 0, 0, 0);

  start_timer(&t_arp, &etharp_tmr, "arp timer", ARP_TMR_INTERVAL);
  start_timer(&t_tcpf, &tcp_fasttmr, "tcp f timer", TCP_FAST_INTERVAL);
  start_timer(&t_tcps, &tcp_slowtmr, "tcp s timer", TCP_SLOW_INTERVAL);
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
#else
void
initnet(void)
{
}
#endif
