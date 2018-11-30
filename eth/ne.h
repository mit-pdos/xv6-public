//
// Migrated from MINIX/drivers/dpeth/8390.h
//

// 1ページのサイズ(byte)
#define DP_PAGESIZE     256     /* NS 8390 page size */

// 送信に必要なページ数
// DST(6) + SRC(6) + LEN(2) + DATA(max:1500) = 1514
#define SENDQ_PAGES     6       /* SENDQ_PAGES * DP_PAGESIZE >= 1514 bytes */

/* Page 0, read/write ------------- */
#define DP_CR           0x00    /* Command Register             RW */
#define DP_CLDA0        0x01    /* Current Local Dma Address 0  RO */
#define DP_PSTART       0x01    /* Page Start Register          WO */
#define DP_CLDA1        0x02    /* Current Local Dma Address 1  RO */
#define DP_PSTOP        0x02    /* Page Stop Register           WO */
#define DP_BNRY         0x03    /* Boundary Pointer             RW */
#define DP_TSR          0x04    /* Transmit Status Register     RO */
#define DP_TPSR         0x04    /* Transmit Page Start Register WO */
#define DP_NCR          0x05    /* No. of Collisions Register   RO */
#define DP_TBCR0        0x05    /* Transmit Byte Count Reg. 0   WO */
#define DP_FIFO         0x06    /* Fifo                         RO */
#define DP_TBCR1        0x06    /* Transmit Byte Count Reg. 1   WO */
#define DP_ISR          0x07    /* Interrupt Status Register    RW */
#define DP_CRDA0        0x08    /* Current Remote Dma Addr.Low  RO */
#define DP_RSAR0        0x08    /* Remote Start Address Low     WO */
#define DP_CRDA1        0x09    /* Current Remote Dma Addr.High RO */
#define DP_RSAR1        0x09    /* Remote Start Address High    WO */
#define DP_RBCR0        0x0A    /* Remote Byte Count Low        WO */
#define DP_RBCR1        0x0B    /* Remote Byte Count Hihg       WO */
#define DP_RSR          0x0C    /* Receive Status Register      RO */
#define DP_RCR          0x0C    /* Receive Config. Register     WO */
#define DP_CNTR0        0x0D    /* Tally Counter 0              RO */
#define DP_TCR          0x0D    /* Transmit Config. Register    WO */
#define DP_CNTR1        0x0E    /* Tally Counter 1              RO */
#define DP_DCR          0x0E    /* Data Configuration Register  WO */
#define DP_CNTR2        0x0F    /* Tally Counter 2              RO */
#define DP_IMR          0x0F    /* Interrupt Mask Register      WO */

/* Page 1, read/write -------------- */
/*      DP_CR           0x00       Command Register */
#define DP_PAR0         0x01    /* Physical Address Register 0 */
#define DP_PAR1         0x02    /* Physical Address Register 1 */
#define DP_PAR2         0x03    /* Physical Address Register 2 */
#define DP_PAR3         0x04    /* Physical Address Register 3 */
#define DP_PAR4         0x05    /* Physical Address Register 4 */
#define DP_PAR5         0x06    /* Physical Address Register 5 */
#define DP_CURR         0x07    /* Current Page Register */
#define DP_MAR0         0x08    /* Multicast Address Register 0 */
#define DP_MAR1         0x09    /* Multicast Address Register 1 */
#define DP_MAR2         0x0A    /* Multicast Address Register 2 */
#define DP_MAR3         0x0B    /* Multicast Address Register 3 */
#define DP_MAR4         0x0C    /* Multicast Address Register 4 */
#define DP_MAR5         0x0D    /* Multicast Address Register 5 */
#define DP_MAR6         0x0E    /* Multicast Address Register 6 */
#define DP_MAR7         0x0F    /* Multicast Address Register 7 */

/* Bits in dp_cr */
#define CR_STP          0x01    /* Stop: software reset */
#define CR_STA          0x02    /* Start: activate NIC */
#define CR_TXP          0x04    /* Transmit Packet */
#define CR_DMA          0x38    /* Mask for DMA control */
#define CR_DM_RR        0x08    /* DMA: Remote Read */
#define CR_DM_RW        0x10    /* DMA: Remote Write */
#define CR_DM_SP        0x18    /* DMA: Send Packet */
#define CR_NO_DMA       0x20    /* DMA: Stop Remote DMA Operation */
#define CR_PS           0xC0    /* Mask for Page Select */
#define CR_PS_P0        0x00    /* Register Page 0 */
#define CR_PS_P1        0x40    /* Register Page 1 */
#define CR_PS_P2        0x80    /* Register Page 2 */

/* Bits in dp_isr */
#define ISR_MASK        0x3F
#define ISR_PRX         0x01    /* Packet Received with no errors */
#define ISR_PTX         0x02    /* Packet Transmitted with no errors */
#define ISR_RXE         0x04    /* Receive Error */
#define ISR_TXE         0x08    /* Transmit Error */
#define ISR_OVW         0x10    /* Overwrite Warning */
#define ISR_CNT         0x20    /* Counter Overflow */
#define ISR_RDC         0x40    /* Remote DMA Complete */
#define ISR_RST         0x80    /* Reset Status */

/* Bits in dp_imr */
#define IMR_PRXE        0x01    /* Packet Received Enable */
#define IMR_PTXE        0x02    /* Packet Transmitted Enable */
#define IMR_RXEE        0x04    /* Receive Error Enable */
#define IMR_TXEE        0x08    /* Transmit Error Enable */
#define IMR_OVWE        0x10    /* Overwrite Warning Enable */
#define IMR_CNTE        0x20    /* Counter Overflow Enable */
#define IMR_RDCE        0x40    /* DMA Complete Enable */

/* Bits in dp_dcr */
#define DCR_WTS         0x01    /* Word Transfer Select */
#define DCR_BYTEWIDE    0x00    /* WTS: byte wide transfers */
#define DCR_WORDWIDE    0x01    /* WTS: word wide transfers */
#define DCR_BOS         0x02    /* Byte Order Select */
#define DCR_LTLENDIAN   0x00    /* BOS: Little Endian */
#define DCR_BIGENDIAN   0x02    /* BOS: Big Endian */
#define DCR_LAS         0x04    /* Long Address Select */
#define DCR_BMS         0x08    /* Burst Mode Select */
#define DCR_AR          0x10    /* Autoinitialize Remote */
#define DCR_FTS         0x60    /* Fifo Threshold Select */
#define DCR_2BYTES      0x00    /* Fifo Threshold: 2 bytes */
#define DCR_4BYTES      0x20    /* Fifo Threshold: 4 bytes */
#define DCR_8BYTES      0x40    /* Fifo Threshold: 8 bytes */
#define DCR_12BYTES     0x60    /* Fifo Threshold: 12 bytes */

/* Bits in dp_tcr */
#define TCR_CRC         0x01    /* Inhibit CRC */
#define TCR_ELC         0x06    /* Encoded Loopback Control */
#define TCR_NORMAL      0x00    /* ELC: Normal Operation */
#define TCR_INTERNAL    0x02    /* ELC: Internal Loopback */
#define TCR_0EXTERNAL   0x04    /* ELC: External Loopback LPBK=0 */
#define TCR_1EXTERNAL   0x06    /* ELC: External Loopback LPBK=1 */
#define TCR_ATD         0x08    /* Auto Transmit */
#define TCR_OFST        0x10    /* Collision Offset Enable */

/* Bits in dp_tsr */
#define TSR_PTX         0x01    /* Packet Transmitted (without error) */
#define TSR_DFR         0x02    /* Transmit Deferred */
#define TSR_COL         0x04    /* Transmit Collided */
#define TSR_ABT         0x08    /* Transmit Aborted */
#define TSR_CRS         0x10    /* Carrier Sense Lost */
#define TSR_FU          0x20    /* FIFO Underrun */
#define TSR_CDH         0x40    /* CD Heartbeat */
#define TSR_OWC         0x80    /* Out of Window Collision */

/* Bits in dp_rcr */
#define RCR_SEP         0x01    /* Save Errored Packets */
#define RCR_AR          0x02    /* Accept Runt Packets */
#define RCR_AB          0x04    /* Accept Broadcast */
#define RCR_AM          0x08    /* Accept Multicast */
#define RCR_PRO         0x10    /* Physical Promiscuous */
#define RCR_MON         0x20    /* Monitor Mode */

/* Bits in dp_rsr */
#define RSR_PRX         0x01    /* Packet Received Intact */
#define RSR_CRC         0x02    /* CRC Error */
#define RSR_FAE         0x04    /* Frame Alignment Error */
#define RSR_FO          0x08    /* FIFO Overrun */
#define RSR_MPA         0x10    /* Missed Packet */
#define RSR_PHY         0x20    /* Multicast Address Match !! */
#define RSR_DIS         0x40    /* Receiver Disabled */

//--------------------------------

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

// 後半16バイトはASICと呼ばれる。
// 0x10-0x17はリモートDMAポート
// 0x18-0x1Fはリセットポート
#define NE_DATA         0x10
#define NE_RESET        0x1F

// NEx000のバッファ使用領域
#define NE1000_START    0x2000  // page 0x20 から
#define NE1000_SIZE     0x2000  // page 0x40 までを送受信バッファとする
#define NE2000_START    0x4000  // page 0x40 から
#define NE2000_SIZE     0x4000  // page 0x80 までを送受信バッファとする

// SENDQ_PAGES * SENDQ_LEN ページを送信バッファとして利用する
#define SENDQ_LEN       2

typedef void(*ne_callback_t)();

typedef struct {
  char name[8];        // 適当な名前
  int irq;             // IRQ
  int base;            // base port
  uchar address[6];    // MAC address
  int is16bit;         // TRUE ? NE2000 : NE1000
  int ramsize;         // = NEx000_SIZE
  int pages;           // = ramsize / DP_PAGESIZE
  int startaddr;       // = NEx000_START
  int send_startpage;  // = NEx000_START / DP_PAGESIZE
  int send_stoppage;   // = send_startpage + SENDQ_PAGES * SENDQ_LEN - 1
  int recv_startpage;  // = send_stoppage + 1
  int recv_stoppage;   // = send_startpage + pages

  // 送信バッファの状態
  struct {
    int filled;        // バッファにパケットが入っているか
    //int size;          // パケットサイズ(何ページ連結する必要があるかとか分かる)
    int sendpage;      // ページ番号(初期化時にセット)
  } sendq[SENDQ_LEN];
  // ひたすら加算していく(SENDQ_LENで余りを取れば要素番号が得られる)
  int sendq_head;      // 初期値 0
  int sendq_tail;      // 初期値 SENDQ_LEN-1
  
} ne_t;


int ne_probe(ne_t* ne);
void ne_init(ne_t* ne);
void ne_rdma_setup(ne_t* ne, int mode, ushort addr, int size);
void ne_getblock(ne_t* ne, ushort addr, int size, void* dst);
void ne_start_xmit(ne_t* ne, int page, int size);
int ne_pio_write(ne_t* ne, uchar* packet, int size);
int ne_pio_read(ne_t* ne, uchar* buf, int size);
void ne_interrupt(ne_t* ne);



