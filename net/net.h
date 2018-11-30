#ifndef NET_NET_H
#define NET_NET_H

typedef unsigned char  u8_t;
typedef unsigned short u16_t;
typedef unsigned int   u32_t;

//-----------------------------------------

#define HTONS(x) (((x >> 8) & 0xFF) | ((x & 0xFF) << 8))
#define HTONL(x)                                \
  (((x >> 24) & 0xFFL) | ((x >> 8) & 0xFF00L) | \
   ((x << 8) & 0xFF0000L) | ((x << 24) & 0xFF000000L))
#define NTOHS(x) HTONS(x)
#define NTOHL(x) HTONL(x)

static inline u16_t htons(u16_t x) { return HTONS(x); }
static inline u32_t htonl(u32_t x) { return HTONL(x); }
static inline u16_t ntohs(u16_t x) { return NTOHS(x); }
static inline u32_t ntohl(u32_t x) { return NTOHL(x); }


//-----------------------------------------

// Max/min packet size
#define ETH_MIN_SIZE    60
#define ETH_MAX_SIZE    1514

typedef struct {
  u8_t dst[6];      // Destination MAC address
  u8_t src[6];      // Source MAC address
  u16_t length;     // Length/type field
} eth_hdr_t;

#define ETH_TYPE_IP4          0x0800
#define ETH_TYPE_IP6          0x86DD
#define ETH_TYPE_ARP          0x0806
#define ETH_TYPE_RARP         0x8035

//-----------------------------------------

typedef struct {
  u8_t ver_ihl;     // Version(1bit) | IP header size(1bit) (usually 0x45)
  u8_t tos;         // Usually 0x00 (because of ignorance)
  u16_t length;     // IP header + IP data size
  u16_t id;         // Identification for fragment data
  u16_t flag_fo;    // Flags(3bit) & fragment offset(13bit)
  u8_t ttl;         // Time to live
  u8_t protocol;    // http://www.iana.org/assignments/protocol-numbers
  u16_t checksum;   // Set 0 first and calcurate later
  u8_t src[4];      // Source IP address
  u8_t dst[4];      // Destination IP address
} ip4_hdr_t;

#define IP_PROTOCOL_ICMP      1
#define IP_PROTOCOL_TCP       6
#define IP_PROTOCOL_UDP       17

//-----------------------------------------

typedef struct {
  u16_t src;        // Source port
  u16_t dst;        // Destination port
  u16_t length;     // UDP header size + UDP data size
  u16_t checksum;   // Set 0 first and calcurate later with a pseudo header
} udp_hdr_t;

#define UDP_PORT_DOMAIN       53      // DNS
#define UDP_PORT_BOOTPS       67      // Boostrap protocol server (DHCP)
#define UDP_PORT_BOOTPC       68      // Boostrap protocol client (DHCP)

//-----------------------------------------

typedef struct {
  u8_t op;          // Op Code
  u8_t htype;       // Hardware type
  u8_t hlen;        // Hardware address length
  u8_t hops;        // Hops
  u32_t xid;        // Transaction ID
  u16_t secs;       // Seconds
  u16_t flags;      // Broadcast flag (1bit) | Must be 0 (7bit)
  u8_t ciaddr[4];   // Client IP address
  u8_t yiaddr[4];   // Your IP address
  u8_t siaddr[4];   // Server IP address
  u8_t giaddr[4];   // Relay (gateway) IP address
  u8_t chaddr[16];  // Client hardware address (MAC address | 0x00 * 10)
  u8_t sname[64];   // Server host name
  u8_t file[128];   // Boot file name
  u32_t magic;      // Magic cookie
  u8_t options[308];// Option field
} dhcp_t;

#define DHCP_OP_BOOTREQUEST   1
#define DHCP_OP_BOOTREPLY     2
#define DHCP_HTYPE_ETH        1
#define DHCP_HLEN_ETH         6
#define DHCP_FLAGS_BCAST      0x8000U
#define DHCP_MAGIC            0x63825363UL

#define DHCP_TAG_HOSTNAME     12
#define DHCP_TAG_VENDER       43
#define DHCP_TAG_REQIP        50
#define DHCP_TAG_TYPE         53
#define DHCP_TAG_REQPAR       55
#define DHCP_TAG_CLASSID      60
#define DHCP_TAG_CLIENTID     61
#define DHCP_TAG_AUTOCONF     116

// DHCP_TAG_TYPE values
#define DHCP_DISCOVER         1
#define DHCP_OFFER            2
#define DHCP_REQUEST          3
#define DHCP_ACK              5
#define DHCP_RELEASE          7

//-----------------------------------------

// set checksum
void ip4_checksum(ip4_hdr_t*);
void udp_checksum(ip4_hdr_t*, udp_hdr_t*, u16_t* data);


#endif /* NET_NET_H */
