#include "net.h"

static u16_t
checksum(const u16_t* buf, int size)
{
  u32_t sum = 0;
  while (size > 1) {
    sum += *buf++;
    size -= 2;
  }
  if (size > 0)
    sum += *(u8_t*)buf;
  sum = (sum & 0xFFFF) + (sum >> 16);
  sum = (sum & 0xFFFF) + (sum >> 16);
  return (u16_t)(~sum);
}

void
ip4_checksum(ip4_hdr_t* hdr)
{
  hdr->checksum = 0;
  hdr->checksum = checksum((u16_t*)hdr, (hdr->ver_ihl & 0xF) * 4);
}

void
udp_checksum(ip4_hdr_t* ip, udp_hdr_t* udp, u16_t* data)
{
  int i;
  u32_t addr;
  u32_t sum = 0;
  int size;

  udp->checksum = 0;

  // pseudo header
  addr = *(u32_t*)ip->src;
  sum += (addr >> 16) & 0xFFFF;
  sum += addr & 0xFFFF;
  addr = *(u32_t*)ip->dst;
  sum += (addr >> 16) & 0xFFFF;
  sum += addr & 0xFFFF;
  sum += htons(ip->protocol);
  sum += udp->length;

  // udp header
  for (i = 0; i < 4; ++i)
    sum += ((u16_t*)udp)[i];

  // payload
  size = ntohs(udp->length) - sizeof(*udp);
  while (size > 1) {
    sum += *data++;
    size -= 2;
  }
  if (size)
    sum += *(u8_t*)data;

  sum = (sum & 0xFFFF) + (sum >> 16);
  sum = (sum & 0xFFFF) + (sum >> 16);
  udp->checksum = (u16_t)~sum;
  return;
}





