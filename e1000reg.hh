/*	$NetBSD: if_wmreg.h,v 1.14.4.1 2005/08/28 09:49:37 tron Exp $	*/

/*
 * Copyright (c) 2001 Wasabi Systems, Inc.
 * All rights reserved.
 *
 * Written by Jason R. Thorpe for Wasabi Systems, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed for the NetBSD Project by
 *	Wasabi Systems, Inc.
 * 4. The name of Wasabi Systems, Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY WASABI SYSTEMS, INC. ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL WASABI SYSTEMS, INC
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Register description for the Intel i82542 (``Wiseman''),
 * i82543 (``Livengood''), and i82544 (``Cordova'') Gigabit
 * Ethernet chips.
 */

/*
 * The Wiseman receive descriptor.
 *
 * The receive descriptor ring must be aligned to a 4K boundary,
 * and there must be an even multiple of 8 descriptors in the ring.
 */
typedef struct wiseman_rxdesc {
	u64	wrx_addr;	/* buffer address */

	u16	wrx_len;	/* buffer length */
	u16	wrx_cksum;	/* checksum (starting at PCSS) */

	u8		wrx_status;	/* Rx status */
	u8		wrx_errors;	/* Rx errors */
	u16	wrx_special;	/* special field (VLAN, etc.) */
} __attribute__((__packed__)) wiseman_rxdesc_t;

/* wrx_status bits */
#define	WRX_ST_DD	(1U << 0)	/* descriptor done */
#define	WRX_ST_EOP	(1U << 1)	/* end of packet */
#define	WRX_ST_IXSM	(1U << 2)	/* ignore checksum indication */
#define	WRX_ST_VP	(1U << 3)	/* VLAN packet */
#define	WRX_ST_BPDU	(1U << 4)	/* ??? */
#define	WRX_ST_TCPCS	(1U << 5)	/* TCP checksum performed */
#define	WRX_ST_IPCS	(1U << 6)	/* IP checksum performed */
#define	WRX_ST_PIF	(1U << 7)	/* passed in-exact filter */

/* wrx_error bits */
#define	WRX_ER_CE	(1U << 0)	/* CRC error */
#define	WRX_ER_SE	(1U << 1)	/* symbol error */
#define	WRX_ER_SEQ	(1U << 2)	/* sequence error */
#define	WRX_ER_ICE	(1U << 3)	/* ??? */
#define	WRX_ER_CXE	(1U << 4)	/* carrier extension error */
#define	WRX_ER_TCPE	(1U << 5)	/* TCP checksum error */
#define	WRX_ER_IPE	(1U << 6)	/* IP checksum error */
#define	WRX_ER_RXE	(1U << 7)	/* Rx data error */

/* wrx_special field for VLAN packets */
#define	WRX_VLAN_ID(x)	((x) & 0x0fff)	/* VLAN identifier */
#define	WRX_VLAN_CFI	(1U << 12)	/* Canonical Form Indicator */
#define	WRX_VLAN_PRI(x)	(((x) >> 13) & 7)/* VLAN priority field */

/*
 * The Wiseman transmit descriptor.
 *
 * The transmit descriptor ring must be aligned to a 4K boundary,
 * and there must be an even multiple of 8 descriptors in the ring.
 */
typedef struct wiseman_tx_fields {
	u8 wtxu_status;		/* Tx status */
	u8 wtxu_options;		/* options */
	u16 wtxu_vlan;		/* VLAN info */
} __attribute__((__packed__)) wiseman_txfields_t;
typedef struct wiseman_txdesc {
	u64	wtx_addr;	/* buffer address */
	u32	wtx_cmdlen;	/* command and length */
	wiseman_txfields_t wtx_fields;	/* fields; see below */
} __attribute__((__packed__)) wiseman_txdesc_t;

/* Commands for wtx_cmdlen */
#define	WTX_CMD_EOP	(1U << 24)	/* end of packet */
#define	WTX_CMD_IFCS	(1U << 25)	/* insert FCS */
#define	WTX_CMD_RS	(1U << 27)	/* report status */
#define	WTX_CMD_RPS	(1U << 28)	/* report packet sent */
#define	WTX_CMD_DEXT	(1U << 29)	/* descriptor extension */
#define	WTX_CMD_VLE	(1U << 30)	/* VLAN enable */
#define	WTX_CMD_IDE	(1U << 31)	/* interrupt delay enable */

/* Descriptor types (if DEXT is set) */
#define	WTX_DTYP_C	(0U << 20)	/* context */
#define	WTX_DTYP_D	(1U << 20)	/* data */

/* wtx_fields status bits */
#define	WTX_ST_DD	(1U << 0)	/* descriptor done */
#define	WTX_ST_EC	(1U << 1)	/* excessive collisions */
#define	WTX_ST_LC	(1U << 2)	/* late collision */
#define	WTX_ST_TU	(1U << 3)	/* transmit underrun */

/* wtx_fields option bits for IP/TCP/UDP checksum offload */
#define	WTX_IXSM	(1U << 0)	/* IP checksum offload */
#define	WTX_TXSM	(1U << 1)	/* TCP/UDP checksum offload */

/* Maximum payload per Tx descriptor */
#define	WTX_MAX_LEN	4096

/*
 * The Livengood TCP/IP context descriptor.
 */
struct livengood_tcpip_ctxdesc {
	u32	tcpip_ipcs;	/* IP checksum context */
	u32	tcpip_tucs;	/* TCP/UDP checksum context */
	u32	tcpip_cmdlen;
	u32	tcpip_seg;	/* TCP segmentation context */
};

/* commands for context descriptors */
#define	WTX_TCPIP_CMD_TCP	(1U << 24)	/* 1 = TCP, 0 = UDP */
#define	WTX_TCPIP_CMD_IP	(1U << 25)	/* 1 = IPv4, 0 = IPv6 */
#define	WTX_TCPIP_CMD_TSE	(1U << 26)	/* segmentation context valid */

#define	WTX_TCPIP_IPCSS(x)	((x) << 0)	/* checksum start */
#define	WTX_TCPIP_IPCSO(x)	((x) << 8)	/* checksum value offset */
#define	WTX_TCPIP_IPCSE(x)	((x) << 16)	/* checksum end */

#define	WTX_TCPIP_TUCSS(x)	((x) << 0)	/* checksum start */
#define	WTX_TCPIP_TUCSO(x)	((x) << 8)	/* checksum value offset */
#define	WTX_TCPIP_TUCSE(x)	((x) << 16)	/* checksum end */

#define	WTX_TCPIP_SEG_STATUS(x)	((x) << 0)
#define	WTX_TCPIP_SEG_HDRLEN(x)	((x) << 8)
#define	WTX_TCPIP_SEG_MSS(x)	((x) << 16)

/*
 * PCI config registers used by the Wiseman.
 */
#define	WM_PCI_MMBA	PCI_MAPREG_START

/*
 * Wiseman Control/Status Registers.
 */
#define	WMREG_CTRL	0x0000	/* Device Control Register */
#define	CTRL_FD		(1U << 0)	/* full duplex */
#define	CTRL_BEM	(1U << 1)	/* big-endian mode */
#define	CTRL_PRIOR	(1U << 2)	/* 0 = receive, 1 = fair */
#define	CTRL_LRST	(1U << 3)	/* link reset */
#define	CTRL_ASDE	(1U << 5)	/* auto speed detect enable */
#define	CTRL_SLU	(1U << 6)	/* set link up */
#define	CTRL_ILOS	(1U << 7)	/* invert loss of signal */
#define	CTRL_SPEED(x)	((x) << 8)	/* speed (Livengood) */
#define	CTRL_SPEED_10	CTRL_SPEED(0)
#define	CTRL_SPEED_100	CTRL_SPEED(1)
#define	CTRL_SPEED_1000	CTRL_SPEED(2)
#define	CTRL_SPEED_MASK	CTRL_SPEED(3)
#define	CTRL_FRCSPD	(1U << 11)	/* force speed (Livengood) */
#define	CTRL_FRCFDX	(1U << 12)	/* force full-duplex (Livengood) */
#define	CTRL_SWDPINS_SHIFT	18
#define	CTRL_SWDPINS_MASK	0x0f
#define	CTRL_SWDPIN(x)		(1U << (CTRL_SWDPINS_SHIFT + (x)))
#define	CTRL_SWDPIO_SHIFT	22
#define	CTRL_SWDPIO_MASK	0x0f
#define	CTRL_SWDPIO(x)		(1U << (CTRL_SWDPIO_SHIFT + (x)))
#define	CTRL_RST	(1U << 26)	/* device reset */
#define	CTRL_RFCE	(1U << 27)	/* Rx flow control enable */
#define	CTRL_TFCE	(1U << 28)	/* Tx flow control enable */
#define	CTRL_VME	(1U << 30)	/* VLAN Mode Enable */
#define	CTRL_PHY_RESET	(1U << 31)	/* PHY reset (Cordova) */

#define	WMREG_CTRL_SHADOW 0x0004	/* Device Control Register (shadow) */

#define	WMREG_STATUS	0x0008	/* Device Status Register */
#define	STATUS_FD	(1U << 0)	/* full duplex */
#define	STATUS_LU	(1U << 1)	/* link up */
#define	STATUS_TCKOK	(1U << 2)	/* Tx clock running */
#define	STATUS_RBCOK	(1U << 3)	/* Rx clock running */
#define	STATUS_FUNCID_SHIFT 2		/* 82546 function ID */
#define	STATUS_FUNCID_MASK  3		/* ... */
#define	STATUS_TXOFF	(1U << 4)	/* Tx paused */
#define	STATUS_TBIMODE	(1U << 5)	/* fiber mode (Livengood) */
#define	STATUS_SPEED(x)	((x) << 6)	/* speed indication */
#define	STATUS_SPEED_10	  STATUS_SPEED(0)
#define	STATUS_SPEED_100  STATUS_SPEED(1)
#define	STATUS_SPEED_1000 STATUS_SPEED(2)
#define	STATUS_ASDV(x)	((x) << 8)	/* auto speed det. val. (Livengood) */
#define	STATUS_MTXCKOK	(1U << 10)	/* MTXD clock running */
#define	STATUS_PCI66	(1U << 11)	/* 66MHz bus (Livengood) */
#define	STATUS_BUS64	(1U << 12)	/* 64-bit bus (Livengood) */
#define	STATUS_PCIX_MODE (1U << 13)	/* PCIX mode (Cordova) */
#define	STATUS_PCIXSPD(x) ((x) << 14)	/* PCIX speed indication (Cordova) */
#define	STATUS_PCIXSPD_50_66   STATUS_PCIXSPD(0)
#define	STATUS_PCIXSPD_66_100  STATUS_PCIXSPD(1)
#define	STATUS_PCIXSPD_100_133 STATUS_PCIXSPD(2)
#define	STATUS_PCIXSPD_MASK    STATUS_PCIXSPD(3)

#define	WMREG_EECD	0x0010	/* EEPROM Control Register */
#define	EECD_SK		(1U << 0)	/* clock */
#define	EECD_CS		(1U << 1)	/* chip select */
#define	EECD_DI		(1U << 2)	/* data in */
#define	EECD_DO		(1U << 3)	/* data out */
#define	EECD_FWE(x)	((x) << 4)	/* flash write enable control */
#define	EECD_FWE_DISABLED EECD_FWE(1)
#define	EECD_FWE_ENABLED  EECD_FWE(2)
#define	EECD_EE_REQ	(1U << 6)	/* (shared) EEPROM request */
#define	EECD_EE_GNT	(1U << 7)	/* (shared) EEPROM grant */
#define	EECD_EE_PRES	(1U << 8)	/* EEPROM present */
#define	EECD_EE_SIZE	(1U << 9)	/* EEPROM size
					   (0 = 64 word, 1 = 256 word) */
#define	EECD_EE_ABITS	(1U << 10)	/* EEPROM address bits
					   (based on type) */
#define	EECD_EE_TYPE	(1U << 13)	/* EEPROM type
					   (0 = Microwire, 1 = SPI) */

#define	UWIRE_OPC_ERASE	0x04		/* MicroWire "erase" opcode */
#define	UWIRE_OPC_WRITE	0x05		/* MicroWire "write" opcode */
#define	UWIRE_OPC_READ	0x06		/* MicroWire "read" opcode */

#define	SPI_OPC_WRITE	0x02		/* SPI "write" opcode */
#define	SPI_OPC_READ	0x03		/* SPI "read" opcode */
#define	SPI_OPC_A8	0x08		/* opcode bit 3 == address bit 8 */
#define	SPI_OPC_WREN	0x06		/* SPI "set write enable" opcode */
#define	SPI_OPC_WRDI	0x04		/* SPI "clear write enable" opcode */
#define	SPI_OPC_RDSR	0x05		/* SPI "read status" opcode */
#define	SPI_OPC_WRSR	0x01		/* SPI "write status" opcode */
#define	SPI_MAX_RETRIES	5000		/* max wait of 5ms for RDY signal */

#define	SPI_SR_RDY	0x01
#define	SPI_SR_WEN	0x02
#define	SPI_SR_BP0	0x04
#define	SPI_SR_BP1	0x08
#define	SPI_SR_WPEN	0x80

#define	EEPROM_OFF_MACADDR	0x00	/* MAC address offset */
#define	EEPROM_OFF_CFG1		0x0a	/* config word 1 */
#define	EEPROM_OFF_CFG2		0x0f	/* config word 2 */
#define	EEPROM_OFF_SWDPIN	0x20	/* SWD Pins (Cordova) */

#define	EEPROM_CFG1_LVDID	(1U << 0)
#define	EEPROM_CFG1_LSSID	(1U << 1)
#define	EEPROM_CFG1_PME_CLOCK	(1U << 2)
#define	EEPROM_CFG1_PM		(1U << 3)
#define	EEPROM_CFG1_ILOS	(1U << 4)
#define	EEPROM_CFG1_SWDPIO_SHIFT 5
#define	EEPROM_CFG1_SWDPIO_MASK	(0xf << EEPROM_CFG1_SWDPIO_SHIFT)
#define	EEPROM_CFG1_IPS1	(1U << 8)
#define	EEPROM_CFG1_LRST	(1U << 9)
#define	EEPROM_CFG1_FD		(1U << 10)
#define	EEPROM_CFG1_FRCSPD	(1U << 11)
#define	EEPROM_CFG1_IPS0	(1U << 12)
#define	EEPROM_CFG1_64_32_BAR	(1U << 13)

#define	EEPROM_CFG2_CSR_RD_SPLIT (1U << 1)
#define	EEPROM_CFG2_APM_EN	(1U << 2)
#define	EEPROM_CFG2_64_BIT	(1U << 3)
#define	EEPROM_CFG2_MAX_READ	(1U << 4)
#define	EEPROM_CFG2_DMCR_MAP	(1U << 5)
#define	EEPROM_CFG2_133_CAP	(1U << 6)
#define	EEPROM_CFG2_MSI_DIS	(1U << 7)
#define	EEPROM_CFG2_FLASH_DIS	(1U << 8)
#define	EEPROM_CFG2_FLASH_SIZE(x) (((x) & 3) >> 9)
#define	EEPROM_CFG2_ANE		(1U << 11)
#define	EEPROM_CFG2_PAUSE(x)	(((x) & 3) >> 12)
#define	EEPROM_CFG2_ASDE	(1U << 14)
#define	EEPROM_CFG2_APM_PME	(1U << 15)
#define	EEPROM_CFG2_SWDPIO_SHIFT 4
#define	EEPROM_CFG2_SWDPIO_MASK	(0xf << EEPROM_CFG2_SWDPIO_SHIFT)

#define	EEPROM_SWDPIN_MASK	0xdf
#define	EEPROM_SWDPIN_SWDPIN_SHIFT 0
#define	EEPROM_SWDPIN_SWDPIO_SHIFT 8

#define WMREG_EERD	0x0014	/* EEPROM read register */
#define EERD_START	0x00000001
#define EERD_DONE	0x00000010
#define EERD_ADDR_SHIFT 8
#define EERD_ADDR_MASK	0x0000ff00
#define EERD_DATA_SHIFT	16
#define EERD_DATA_MASK	0xffff0000

#define	WMREG_CTRL_EXT	0x0018	/* Extended Device Control Register */
#define	CTRL_EXT_GPI_EN(x)	(1U << (x)) /* gpin interrupt enable */
#define	CTRL_EXT_SWDPINS_SHIFT	4
#define	CTRL_EXT_SWDPINS_MASK	0x0d
#define	CTRL_EXT_SWDPIN(x)	(1U << (CTRL_EXT_SWDPINS_SHIFT + (x) - 4))
#define	CTRL_EXT_SWDPIO_SHIFT	8
#define	CTRL_EXT_SWDPIO_MASK	0x0d
#define	CTRL_EXT_SWDPIO(x)	(1U << (CTRL_EXT_SWDPIO_SHIFT + (x) - 4))
#define	CTRL_EXT_ASDCHK		(1U << 12) /* ASD check */
#define	CTRL_EXT_EE_RST		(1U << 13) /* EEPROM reset */
#define	CTRL_EXT_IPS		(1U << 14) /* invert power state bit 0 */
#define	CTRL_EXT_SPD_BYPS	(1U << 15) /* speed select bypass */
#define	CTRL_EXT_IPS1		(1U << 16) /* invert power state bit 1 */
#define	CTRL_EXT_RO_DIS		(1U << 17) /* relaxed ordering disabled */

#define	WMREG_MDIC	0x0020	/* MDI Control Register */
#define	MDIC_DATA(x)	((x) & 0xffff)
#define	MDIC_REGADD(x)	((x) << 16)
#define	MDIC_PHYADD(x)	((x) << 21)
#define	MDIC_OP_WRITE	(1U << 26)
#define	MDIC_OP_READ	(2U << 26)
#define	MDIC_READY	(1U << 28)
#define	MDIC_I		(1U << 29)	/* interrupt on MDI complete */
#define	MDIC_E		(1U << 30)	/* MDI error */

#define	WMREG_FCAL	0x0028	/* Flow Control Address Low */
#define	FCAL_CONST	0x00c28001	/* Flow Control MAC addr low */

#define	WMREG_FCAH	0x002c	/* Flow Control Address High */
#define	FCAH_CONST	0x00000100	/* Flow Control MAC addr high */

#define	WMREG_FCT	0x0030	/* Flow Control Type */

#define	WMREG_VET	0x0038	/* VLAN Ethertype */

#define	WMREG_RAL_BASE	0x0040	/* Receive Address List */
#define	WMREG_CORDOVA_RAL_BASE 0x5400
#define	WMREG_RAL_LO(b, x) ((b) + ((x) << 3))
#define	WMREG_RAL_HI(b, x) (WMREG_RAL_LO(b, x) + 4)
	/*
	 * Receive Address List: The LO part is the low-order 32-bits
	 * of the MAC address.  The HI part is the high-order 16-bits
	 * along with a few control bits.
	 */
#define	RAL_AS(x)	((x) << 16)	/* address select */
#define	RAL_AS_DEST	RAL_AS(0)	/* (cordova?) */
#define	RAL_AS_SOURCE	RAL_AS(1)	/* (cordova?) */
#define	RAL_RDR1	(1U << 30)	/* put packet in alt. rx ring */
#define	RAL_AV		(1U << 31)	/* entry is valid */

#define	WM_RAL_TABSIZE	16

#define	WMREG_ICR	0x00c0	/* Interrupt Cause Register */
#define	ICR_TXDW	(1U << 0)	/* Tx desc written back */
#define	ICR_TXQE	(1U << 1)	/* Tx queue empty */
#define	ICR_LSC		(1U << 2)	/* link status change */
#define	ICR_RXSEQ	(1U << 3)	/* receive sequence error */
#define	ICR_RXDMT0	(1U << 4)	/* Rx ring 0 nearly empty */
#define	ICR_RXO		(1U << 6)	/* Rx overrun */
#define	ICR_RXT0	(1U << 7)	/* Rx ring 0 timer */
#define	ICR_MDAC	(1U << 9)	/* MDIO access complete */
#define	ICR_RXCFG	(1U << 10)	/* Receiving /C/ */
#define	ICR_GPI(x)	(1U << (x))	/* general purpose interrupts */

#define WMREG_ITR	0x00c4	/* Interrupt Throttling Register */
#define ITR_IVAL_MASK	0xffff		/* Interval mask */
#define ITR_IVAL_SHIFT	0		/* Interval shift */

#define	WMREG_ICS	0x00c8	/* Interrupt Cause Set Register */
	/* See ICR bits. */

#define	WMREG_IMS	0x00d0	/* Interrupt Mask Set Register */
	/* See ICR bits. */

#define	WMREG_IMC	0x00d8	/* Interrupt Mask Clear Register */
	/* See ICR bits. */

#define	WMREG_RCTL	0x0100	/* Receive Control */
#define	RCTL_EN		(1U << 1)	/* receiver enable */
#define	RCTL_SBP	(1U << 2)	/* store bad packets */
#define	RCTL_UPE	(1U << 3)	/* unicast promisc. enable */
#define	RCTL_MPE	(1U << 4)	/* multicast promisc. enable */
#define	RCTL_LPE	(1U << 5)	/* large packet enable */
#define	RCTL_LBM(x)	((x) << 6)	/* loopback mode */
#define	RCTL_LBM_NONE	RCTL_LBM(0)
#define	RCTL_LBM_PHY	RCTL_LBM(3)
#define	RCTL_RDMTS(x)	((x) << 8)	/* receive desc. min thresh size */
#define	RCTL_RDMTS_1_2	RCTL_RDMTS(0)
#define	RCTL_RDMTS_1_4	RCTL_RDMTS(1)
#define	RCTL_RDMTS_1_8	RCTL_RDMTS(2)
#define	RCTL_RDMTS_MASK	RCTL_RDMTS(3)
#define	RCTL_MO(x)	((x) << 12)	/* multicast offset */
#define	RCTL_BAM	(1U << 15)	/* broadcast accept mode */
#define	RCTL_2k		(0 << 16)	/* 2k Rx buffers */
#define	RCTL_1k		(1 << 16)	/* 1k Rx buffers */
#define	RCTL_512	(2 << 16)	/* 512 byte Rx buffers */
#define	RCTL_256	(3 << 16)	/* 256 byte Rx buffers */
#define	RCTL_BSEX_16k	(1 << 16)	/* 16k Rx buffers (BSEX) */
#define	RCTL_BSEX_8k	(2 << 16)	/* 8k Rx buffers (BSEX) */
#define	RCTL_BSEX_4k	(3 << 16)	/* 4k Rx buffers (BSEX) */
#define	RCTL_DPF	(1U << 22)	/* discard pause frames */
#define	RCTL_PMCF	(1U << 23)	/* pass MAC control frames */
#define	RCTL_BSEX	(1U << 25)	/* buffer size extension (Livengood) */
#define	RCTL_SECRC	(1U << 26)	/* strip Ethernet CRC */

#define	WMREG_OLD_RDTR0	0x0108	/* Receive Delay Timer (ring 0) */
#define	WMREG_RDTR	0x2820
#define	RDTR_FPD	(1U << 31)	/* flush partial descriptor */

#define	WMREG_RADV	0x282c	/* Receive Interrupt Absolute Delay Timer */

#define	WMREG_OLD_RDBAL0 0x0110	/* Receive Descriptor Base Low (ring 0) */
#define	WMREG_RDBAL	0x2800

#define	WMREG_OLD_RDBAH0 0x0114	/* Receive Descriptor Base High (ring 0) */
#define	WMREG_RDBAH	0x2804

#define	WMREG_OLD_RDLEN0 0x0118	/* Receive Descriptor Length (ring 0) */
#define	WMREG_RDLEN	0x2808

#define	WMREG_OLD_RDH0	0x0120	/* Receive Descriptor Head (ring 0) */
#define	WMREG_RDH	0x2810

#define	WMREG_OLD_RDT0	0x0128	/* Receive Descriptor Tail (ring 0) */
#define	WMREG_RDT	0x2818

#define	WMREG_RXDCTL	0x2828	/* Receive Descriptor Control */
#define	RXDCTL_PTHRESH(x) ((x) << 0)	/* prefetch threshold */
#define	RXDCTL_HTHRESH(x) ((x) << 8)	/* host threshold */
#define	RXDCTL_WTHRESH(x) ((x) << 16)	/* write back threshold */
#define	RXDCTL_GRAN	(1U << 24)	/* 0 = cacheline, 1 = descriptor */

#define	WMREG_OLD_RDTR1	0x0130	/* Receive Delay Timer (ring 1) */

#define	WMREG_OLD_RDBA1_LO 0x0138 /* Receive Descriptor Base Low (ring 1) */

#define	WMREG_OLD_RDBA1_HI 0x013c /* Receive Descriptor Base High (ring 1) */

#define	WMREG_OLD_RDLEN1 0x0140	/* Receive Drscriptor Length (ring 1) */

#define	WMREG_OLD_RDH1	0x0148

#define	WMREG_OLD_RDT1	0x0150

#define	WMREG_OLD_FCRTH 0x0160	/* Flow Control Rx Threshold Hi (OLD) */
#define	WMREG_FCRTL	0x2160	/* Flow Control Rx Threshold Lo */
#define	FCRTH_DFLT	0x00008000

#define	WMREG_OLD_FCRTL 0x0168	/* Flow Control Rx Threshold Lo (OLD) */
#define	WMREG_FCRTH	0x2168	/* Flow Control Rx Threhsold Hi */
#define	FCRTL_DFLT	0x00004000
#define	FCRTL_XONE	0x80000000	/* Enable XON frame transmission */

#define	WMREG_FCTTV	0x0170	/* Flow Control Transmit Timer Value */
#define	FCTTV_DFLT	0x00000600

#define	WMREG_TXCW	0x0178	/* Transmit Configuration Word (TBI mode) */
	/* See MII ANAR_X bits. */
#define	TXCW_TxConfig	(1U << 30)	/* Tx Config */
#define	TXCW_ANE	(1U << 31)	/* Autonegotiate */

#define	WMREG_RXCW	0x0180	/* Receive Configuration Word (TBI mode) */
	/* See MII ANLPAR_X bits. */
#define	RXCW_NC		(1U << 26)	/* no carrier */
#define	RXCW_IV		(1U << 27)	/* config invalid */
#define	RXCW_CC		(1U << 28)	/* config change */
#define	RXCW_C		(1U << 29)	/* /C/ reception */
#define	RXCW_SYNCH	(1U << 30)	/* synchronized */
#define	RXCW_ANC	(1U << 31)	/* autonegotiation complete */

#define	WMREG_MTA	0x0200	/* Multicast Table Array */
#define	WMREG_CORDOVA_MTA 0x5200

#define	WMREG_TCTL	0x0400	/* Transmit Control Register */
#define	TCTL_EN		(1U << 1)	/* transmitter enable */
#define	TCTL_PSP	(1U << 3)	/* pad short packets */
#define	TCTL_CT(x)	(((x) & 0xff) << 4)   /* 4:11 - collision threshold */
#define	TCTL_COLD(x)	(((x) & 0x3ff) << 12) /* 12:21 - collision distance */
#define	TCTL_SWXOFF	(1U << 22)	/* software XOFF */
#define	TCTL_RTLC	(1U << 24)	/* retransmit on late collision */
#define	TCTL_NRTU	(1U << 25)	/* no retransmit on underrun */

#define	TX_COLLISION_THRESHOLD		15
#define	TX_COLLISION_DISTANCE_HDX	64
#define	TX_COLLISION_DISTANCE_FDX	512

#define	WMREG_TQSA_LO	0x0408

#define	WMREG_TQSA_HI	0x040c

#define	WMREG_TIPG	0x0410	/* Transmit IPG Register */
#define	TIPG_IPGT(x)	(x)		/* IPG transmit time */
#define	TIPG_IPGR1(x)	((x) << 10)	/* IPG receive time 1 */
#define	TIPG_IPGR2(x)	((x) << 20)	/* IPG receive time 2 */

#define	TIPG_WM_DFLT	(TIPG_IPGT(0x0a) | TIPG_IPGR1(0x02) | TIPG_IPGR2(0x0a))
#define	TIPG_LG_DFLT	(TIPG_IPGT(0x06) | TIPG_IPGR1(0x08) | TIPG_IPGR2(0x06))
#define	TIPG_1000T_DFLT	(TIPG_IPGT(0x08) | TIPG_IPGR1(0x08) | TIPG_IPGR2(0x06))

#define	WMREG_TQC	0x0418

#define	WMREG_RDFH	0x2410	/* Receive Data FIFO Head */

#define	WMREG_RDFT	0x2418	/* Receive Data FIFO Tail */

#define	WMREG_RDFHS	0x2420	/* Receive Data FIFO Head Saved */

#define	WMREG_RDFTS	0x2428	/* Receive Data FIFO Tail Saved */

#define	WMREG_TDFH	0x3410	/* Transmit Data FIFO Head */

#define	WMREG_TDFT	0x3418	/* Transmit Data FIFO Tail */

#define	WMREG_TDFHS	0x3420	/* Transmit Data FIFO Head Saved */

#define	WMREG_TDFTS	0x3428	/* Transmit Data FIFO Tail Saved */

#define	WMREG_TDFPC	0x3430	/* Transmit Data FIFO Packet Count */

#define	WMREG_OLD_TDBAL	0x0420	/* Transmit Descriptor Base Lo */
#define	WMREG_TDBAL	0x3800

#define	WMREG_OLD_TDBAH	0x0424	/* Transmit Descriptor Base Hi */
#define	WMREG_TDBAH	0x3804

#define	WMREG_OLD_TDLEN	0x0428	/* Transmit Descriptor Length */
#define	WMREG_TDLEN	0x3808

#define	WMREG_OLD_TDH	0x0430	/* Transmit Descriptor Head */
#define	WMREG_TDH	0x3810

#define	WMREG_OLD_TDT	0x0438	/* Transmit Descriptor Tail */
#define	WMREG_TDT	0x3818

#define	WMREG_OLD_TIDV	0x0440	/* Transmit Delay Interrupt Value */
#define	WMREG_TIDV	0x3820

#define	WMREG_TXDCTL	0x3828	/* Trandmit Descriptor Control */
#define	TXDCTL_PTHRESH(x) ((x) << 0)	/* prefetch threshold */
#define	TXDCTL_HTHRESH(x) ((x) << 8)	/* host threshold */
#define	TXDCTL_WTHRESH(x) ((x) << 16)	/* write back threshold */

#define	WMREG_TADV	0x382c	/* Transmit Absolute Interrupt Delay Timer */

#define	WMREG_AIT	0x0458	/* Adaptive IFS Throttle */

#define	WMREG_VFTA	0x0600

#define	WM_MC_TABSIZE	128
#define	WM_VLAN_TABSIZE	128

#define	WMREG_PBA	0x1000	/* Packet Buffer Allocation */
#define	PBA_RX_MASK	0xffff
#define	PBA_RX_SHIFT	0
#define	PBA_TX_MASK	0xffff
#define	PBA_TX_SHIFT	16

#define	WMREG_TXDMAC	0x3000	/* Transfer DMA Control */
#define	TXDMAC_DPP	(1U << 0)	/* disable packet prefetch */

#define	WMREG_TSPMT	0x3830	/* TCP Segmentation Pad and Minimum
				   Threshold (Cordova) */
#define	TSPMT_TSMT(x)	(x)		/* TCP seg min transfer */
#define	TSPMT_TSPBP(x)	((x) << 16)	/* TCP seg pkt buf padding */

#define	WMREG_RXCSUM	0x5000	/* Receive Checksum register */
#define	RXCSUM_PCSS	0x000000ff	/* Packet Checksum Start */
#define	RXCSUM_IPOFL	(1U << 8)	/* IP checksum offload */
#define	RXCSUM_TUOFL	(1U << 9)	/* TCP/UDP checksum offload */

#define	WMREG_XONRXC	0x4048	/* XON Rx Count - R/clr */
#define	WMREG_XONTXC	0x404c	/* XON Tx Count - R/clr */
#define	WMREG_XOFFRXC	0x4050	/* XOFF Rx Count - R/clr */
#define	WMREG_XOFFTXC	0x4054	/* XOFF Tx Count - R/clr */
#define	WMREG_FCRUC	0x4058	/* Flow Control Rx Unsupported Count - R/clr */

#define WMREG_MANC	0x5820	/* Management control */
#define MANC_ARP_REQ	0x00002000	/* ARP request filtering */
#define MANC_ARP_RES	0x00008000	/* ARP response filtering */
#define MANC_MNG2HOST	0x00200000	/* Management packets to host */

#define WMREG_MANC2H	0x5860	/* Management control to host */
#define MANC2H_PORT_623	0x0020
#define MANC2H_PORT_664	0x0040

