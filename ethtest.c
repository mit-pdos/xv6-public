#include "types.h"
#include "fcntl.h"
#include "user.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "net/net.h"
#include "eth/eth.h"

void dump(const u8_t* buf, int size);

#define MAC_ADDRESS 0x52, 0x54, 0x00, 0x12, 0x34, 0x56

void
getip(int fd)
{
  // DHCPパケットを作成し、IPアドレスを取得
  static eth_hdr_t ethhdr = {
    dst: { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
    src: { MAC_ADDRESS },
    length: HTONS(ETH_TYPE_IP4),
  };
  static ip4_hdr_t iphdr = {
    ver_ihl: 0x45,
    tos: 0,
    length: 0, // calc later
    id: 0,
    flag_fo: 0,
    ttl: 0xFF,
    protocol: IP_PROTOCOL_UDP,
    checksum: 0,
    src: { 0x00, 0x00, 0x00, 0x00 }, // none
    dst: { 0xFF, 0xFF, 0xFF, 0xFF }, // broadcast
  };
  static udp_hdr_t udphdr = {
    src: HTONS(UDP_PORT_BOOTPC),
    dst: HTONS(UDP_PORT_BOOTPS),
    length: 0, // calc later
    checksum: 0,
  };
  static dhcp_t dhcp = {
    op: DHCP_OP_BOOTREQUEST,
    htype: DHCP_HTYPE_ETH,
    hlen: DHCP_HLEN_ETH,
    xid: 0,
    flags: DHCP_FLAGS_BCAST,
    chaddr: { MAC_ADDRESS },
    magic: HTONL(DHCP_MAGIC),
    options: {
      DHCP_TAG_TYPE, 1, DHCP_DISCOVER,
      DHCP_TAG_CLIENTID, 7, 1, MAC_ADDRESS,
      0xFF, // stopper
    },
  };
  uchar* t;
  static uchar buf[ETH_MAX_SIZE];
  int size;
  
  size = sizeof(udphdr) + sizeof(dhcp) - sizeof(dhcp.options) + 13;
  udphdr.length = htons(size);
  size += sizeof(iphdr);
  iphdr.length = htons(size);
  
  ip4_checksum(&iphdr);
  udp_checksum(&iphdr, &udphdr, (u16_t*)&dhcp);
  
  t = buf;
  memmove(t, &ethhdr, sizeof(ethhdr));
  t += sizeof(ethhdr);
  memmove(t, &iphdr, sizeof(iphdr));
  t += sizeof(iphdr);
  memmove(t, &udphdr, sizeof(udphdr));
  t += sizeof(udphdr);
  memmove(t, &dhcp, sizeof(dhcp));

  size += sizeof(ethhdr);
  printf(1, "write %d byte\n", write(fd, buf, size));
  //dump(buf, size);

  while ((size = read(fd, buf, ETH_MAX_SIZE)) == 0)
    sleep(10);
  if (size == -1)
    return;
    
  printf(1, "receive %d byte\n", size);
  //dump(buf, size);
  {
    int i;
    dhcp_t* d;
    d = (dhcp_t*)(buf + sizeof(ethhdr) + sizeof(iphdr) + sizeof(udphdr));
    printf(1, "Given IPv4 address: ");
    for (i = 0; i < 4; ++i)
      printf(1, "%d%s", d->yiaddr[i], i == 3 ? "\n" : ".");
  }
}

void
ethtest(int fd)
{
  ioctl(fd, ETH_IPC_SETUP, 0); // ioctl test.
  getip(fd);
}

int
main(int argc, char** argv)
{
  int fd = open("eth", O_RDWR);
  if (fd < 0) {
    printf(1, "open: cannot open %s\n", "eth");
    exit();
  }
  ethtest(fd);
  close(fd);
  exit();
  return 0;
}

//-----------------------------------

void
dump(const u8_t* buf, int size)
{
  int i;
  const int BR = 16;
  for (i = 0; i < size; ++i) {
    if (i != 0) {
      if (i % BR == 0)
        printf(1, "\n");
      else if (i*2 % BR == 0)
        printf(1, " ");
    }
    printf(1, (buf[i] <= 0xF ? "0%x " : "%x "), buf[i]);
  }
  printf(1, "\n");
}





