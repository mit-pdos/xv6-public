/*	$NetBSD: pcireg.h,v 1.45 2004/02/04 06:58:24 soren Exp $	*/

/*
 * Copyright (c) 1995, 1996, 1999, 2000
 *     Christopher G. Demetriou.  All rights reserved.
 * Copyright (c) 1994, 1996 Charles M. Hannum.  All rights reserved.
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
 *	This product includes software developed by Charles M. Hannum.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DEV_PCI_PCIREG_H_
#define	_DEV_PCI_PCIREG_H_

/*
 * Standardized PCI configuration information
 *
 * XXX This is not complete.
 */

/*
 * Device identification register; contains a vendor ID and a device ID.
 */
#define	PCI_ID_REG			0x00

typedef u16 pci_vendor_id_t;
typedef u16 pci_product_id_t;

#define	PCI_VENDOR_SHIFT			0
#define	PCI_VENDOR_MASK				0xffff
#define	PCI_VENDOR(id) \
	    (((id) >> PCI_VENDOR_SHIFT) & PCI_VENDOR_MASK)

#define	PCI_PRODUCT_SHIFT			16
#define	PCI_PRODUCT_MASK			0xffff
#define	PCI_PRODUCT(id) \
	    (((id) >> PCI_PRODUCT_SHIFT) & PCI_PRODUCT_MASK)

#define PCI_ID_CODE(vid,pid)					\
	((((vid) & PCI_VENDOR_MASK) << PCI_VENDOR_SHIFT) |	\
	 (((pid) & PCI_PRODUCT_MASK) << PCI_PRODUCT_SHIFT))	\

/*
 * Command and status register.
 */
#define	PCI_COMMAND_STATUS_REG			0x04
#define	PCI_COMMAND_SHIFT			0
#define	PCI_COMMAND_MASK			0xffff
#define	PCI_STATUS_SHIFT			16
#define	PCI_STATUS_MASK				0xffff

#define PCI_COMMAND_STATUS_CODE(cmd,stat)			\
	((((cmd) & PCI_COMMAND_MASK) >> PCI_COMMAND_SHIFT) |	\
	 (((stat) & PCI_STATUS_MASK) >> PCI_STATUS_SHIFT))	\

#define	PCI_COMMAND_IO_ENABLE			0x00000001
#define	PCI_COMMAND_MEM_ENABLE			0x00000002
#define	PCI_COMMAND_MASTER_ENABLE		0x00000004
#define	PCI_COMMAND_SPECIAL_ENABLE		0x00000008
#define	PCI_COMMAND_INVALIDATE_ENABLE		0x00000010
#define	PCI_COMMAND_PALETTE_ENABLE		0x00000020
#define	PCI_COMMAND_PARITY_ENABLE		0x00000040
#define	PCI_COMMAND_STEPPING_ENABLE		0x00000080
#define	PCI_COMMAND_SERR_ENABLE			0x00000100
#define	PCI_COMMAND_BACKTOBACK_ENABLE		0x00000200

#define	PCI_STATUS_CAPLIST_SUPPORT		0x00100000
#define	PCI_STATUS_66MHZ_SUPPORT		0x00200000
#define	PCI_STATUS_UDF_SUPPORT			0x00400000
#define	PCI_STATUS_BACKTOBACK_SUPPORT		0x00800000
#define	PCI_STATUS_PARITY_ERROR			0x01000000
#define	PCI_STATUS_DEVSEL_FAST			0x00000000
#define	PCI_STATUS_DEVSEL_MEDIUM		0x02000000
#define	PCI_STATUS_DEVSEL_SLOW			0x04000000
#define	PCI_STATUS_DEVSEL_MASK			0x06000000
#define	PCI_STATUS_TARGET_TARGET_ABORT		0x08000000
#define	PCI_STATUS_MASTER_TARGET_ABORT		0x10000000
#define	PCI_STATUS_MASTER_ABORT			0x20000000
#define	PCI_STATUS_SPECIAL_ERROR		0x40000000
#define	PCI_STATUS_PARITY_DETECT		0x80000000

/*
 * PCI Class and Revision Register; defines type and revision of device.
 */
#define	PCI_CLASS_REG			0x08

typedef u8 pci_class_t;
typedef u8 pci_subclass_t;
typedef u8 pci_interface_t;
typedef u8 pci_revision_t;

#define	PCI_CLASS_SHIFT				24
#define	PCI_CLASS_MASK				0xff
#define	PCI_CLASS(cr) \
	    (((cr) >> PCI_CLASS_SHIFT) & PCI_CLASS_MASK)

#define	PCI_SUBCLASS_SHIFT			16
#define	PCI_SUBCLASS_MASK			0xff
#define	PCI_SUBCLASS(cr) \
	    (((cr) >> PCI_SUBCLASS_SHIFT) & PCI_SUBCLASS_MASK)

#define	PCI_INTERFACE_SHIFT			8
#define	PCI_INTERFACE_MASK			0xff
#define	PCI_INTERFACE(cr) \
	    (((cr) >> PCI_INTERFACE_SHIFT) & PCI_INTERFACE_MASK)

#define	PCI_REVISION_SHIFT			0
#define	PCI_REVISION_MASK			0xff
#define	PCI_REVISION(cr) \
	    (((cr) >> PCI_REVISION_SHIFT) & PCI_REVISION_MASK)

#define	PCI_CLASS_CODE(mainclass, subclass, interface) \
	    ((((mainclass) & PCI_CLASS_MASK) << PCI_CLASS_SHIFT) | \
	     (((subclass) & PCI_SUBCLASS_MASK) << PCI_SUBCLASS_SHIFT) | \
	     (((interface) & PCI_INTERFACE_MASK) << PCI_INTERFACE_SHIFT))

/* base classes */
#define	PCI_CLASS_PREHISTORIC			0x00
#define	PCI_CLASS_MASS_STORAGE			0x01
#define	PCI_CLASS_NETWORK			0x02
#define	PCI_CLASS_DISPLAY			0x03
#define	PCI_CLASS_MULTIMEDIA			0x04
#define	PCI_CLASS_MEMORY			0x05
#define	PCI_CLASS_BRIDGE			0x06
#define	PCI_CLASS_COMMUNICATIONS		0x07
#define	PCI_CLASS_SYSTEM			0x08
#define	PCI_CLASS_INPUT				0x09
#define	PCI_CLASS_DOCK				0x0a
#define	PCI_CLASS_PROCESSOR			0x0b
#define	PCI_CLASS_SERIALBUS			0x0c
#define	PCI_CLASS_WIRELESS			0x0d
#define	PCI_CLASS_I2O				0x0e
#define	PCI_CLASS_SATCOM			0x0f
#define	PCI_CLASS_CRYPTO			0x10
#define	PCI_CLASS_DASP				0x11
#define	PCI_CLASS_UNDEFINED			0xff

/* 0x00 prehistoric subclasses */
#define	PCI_SUBCLASS_PREHISTORIC_MISC		0x00
#define	PCI_SUBCLASS_PREHISTORIC_VGA		0x01

/* 0x01 mass storage subclasses */
#define	PCI_SUBCLASS_MASS_STORAGE_SCSI		0x00
#define	PCI_SUBCLASS_MASS_STORAGE_IDE		0x01
#define	PCI_SUBCLASS_MASS_STORAGE_FLOPPY	0x02
#define	PCI_SUBCLASS_MASS_STORAGE_IPI		0x03
#define	PCI_SUBCLASS_MASS_STORAGE_RAID		0x04
#define	PCI_SUBCLASS_MASS_STORAGE_ATA		0x05
#define	PCI_SUBCLASS_MASS_STORAGE_SATA		0x06
#define	PCI_SUBCLASS_MASS_STORAGE_MISC		0x80

/* 0x02 network subclasses */
#define	PCI_SUBCLASS_NETWORK_ETHERNET		0x00
#define	PCI_SUBCLASS_NETWORK_TOKENRING		0x01
#define	PCI_SUBCLASS_NETWORK_FDDI		0x02
#define	PCI_SUBCLASS_NETWORK_ATM		0x03
#define	PCI_SUBCLASS_NETWORK_ISDN		0x04
#define	PCI_SUBCLASS_NETWORK_WORLDFIP		0x05
#define	PCI_SUBCLASS_NETWORK_PCIMGMULTICOMP	0x06
#define	PCI_SUBCLASS_NETWORK_MISC		0x80

/* 0x03 display subclasses */
#define	PCI_SUBCLASS_DISPLAY_VGA		0x00
#define	PCI_SUBCLASS_DISPLAY_XGA		0x01
#define	PCI_SUBCLASS_DISPLAY_3D			0x02
#define	PCI_SUBCLASS_DISPLAY_MISC		0x80

/* 0x04 multimedia subclasses */
#define	PCI_SUBCLASS_MULTIMEDIA_VIDEO		0x00
#define	PCI_SUBCLASS_MULTIMEDIA_AUDIO		0x01
#define	PCI_SUBCLASS_MULTIMEDIA_TELEPHONY	0x02
#define	PCI_SUBCLASS_MULTIMEDIA_MISC		0x80

/* 0x05 memory subclasses */
#define	PCI_SUBCLASS_MEMORY_RAM			0x00
#define	PCI_SUBCLASS_MEMORY_FLASH		0x01
#define	PCI_SUBCLASS_MEMORY_MISC		0x80

/* 0x06 bridge subclasses */
#define	PCI_SUBCLASS_BRIDGE_HOST		0x00
#define	PCI_SUBCLASS_BRIDGE_ISA			0x01
#define	PCI_SUBCLASS_BRIDGE_EISA		0x02
#define	PCI_SUBCLASS_BRIDGE_MC			0x03	/* XXX _MCA? */
#define	PCI_SUBCLASS_BRIDGE_PCI			0x04
#define	PCI_SUBCLASS_BRIDGE_PCMCIA		0x05
#define	PCI_SUBCLASS_BRIDGE_NUBUS		0x06
#define	PCI_SUBCLASS_BRIDGE_CARDBUS		0x07
#define	PCI_SUBCLASS_BRIDGE_RACEWAY		0x08
#define	PCI_SUBCLASS_BRIDGE_STPCI		0x09
#define	PCI_SUBCLASS_BRIDGE_INFINIBAND		0x0a
#define	PCI_SUBCLASS_BRIDGE_MISC		0x80

/* 0x07 communications subclasses */
#define	PCI_SUBCLASS_COMMUNICATIONS_SERIAL	0x00
#define	PCI_SUBCLASS_COMMUNICATIONS_PARALLEL	0x01
#define	PCI_SUBCLASS_COMMUNICATIONS_MPSERIAL	0x02
#define	PCI_SUBCLASS_COMMUNICATIONS_MODEM	0x03
#define	PCI_SUBCLASS_COMMUNICATIONS_GPIB	0x04
#define	PCI_SUBCLASS_COMMUNICATIONS_SMARTCARD	0x05
#define	PCI_SUBCLASS_COMMUNICATIONS_MISC	0x80

/* 0x08 system subclasses */
#define	PCI_SUBCLASS_SYSTEM_PIC			0x00
#define	PCI_SUBCLASS_SYSTEM_DMA			0x01
#define	PCI_SUBCLASS_SYSTEM_TIMER		0x02
#define	PCI_SUBCLASS_SYSTEM_RTC			0x03
#define	PCI_SUBCLASS_SYSTEM_PCIHOTPLUG		0x04
#define	PCI_SUBCLASS_SYSTEM_MISC		0x80

/* 0x09 input subclasses */
#define	PCI_SUBCLASS_INPUT_KEYBOARD		0x00
#define	PCI_SUBCLASS_INPUT_DIGITIZER		0x01
#define	PCI_SUBCLASS_INPUT_MOUSE		0x02
#define	PCI_SUBCLASS_INPUT_SCANNER		0x03
#define	PCI_SUBCLASS_INPUT_GAMEPORT		0x04
#define	PCI_SUBCLASS_INPUT_MISC			0x80

/* 0x0a dock subclasses */
#define	PCI_SUBCLASS_DOCK_GENERIC		0x00
#define	PCI_SUBCLASS_DOCK_MISC			0x80

/* 0x0b processor subclasses */
#define	PCI_SUBCLASS_PROCESSOR_386		0x00
#define	PCI_SUBCLASS_PROCESSOR_486		0x01
#define	PCI_SUBCLASS_PROCESSOR_PENTIUM		0x02
#define	PCI_SUBCLASS_PROCESSOR_ALPHA		0x10
#define	PCI_SUBCLASS_PROCESSOR_POWERPC		0x20
#define	PCI_SUBCLASS_PROCESSOR_MIPS		0x30
#define	PCI_SUBCLASS_PROCESSOR_COPROC		0x40

/* 0x0c serial bus subclasses */
#define	PCI_SUBCLASS_SERIALBUS_FIREWIRE		0x00
#define	PCI_SUBCLASS_SERIALBUS_ACCESS		0x01
#define	PCI_SUBCLASS_SERIALBUS_SSA		0x02
#define	PCI_SUBCLASS_SERIALBUS_USB		0x03
#define	PCI_SUBCLASS_SERIALBUS_FIBER		0x04	/* XXX _FIBRECHANNEL */
#define	PCI_SUBCLASS_SERIALBUS_SMBUS		0x05
#define	PCI_SUBCLASS_SERIALBUS_INFINIBAND	0x06
#define	PCI_SUBCLASS_SERIALBUS_IPMI		0x07
#define	PCI_SUBCLASS_SERIALBUS_SERCOS		0x08
#define	PCI_SUBCLASS_SERIALBUS_CANBUS		0x09

/* 0x0d wireless subclasses */
#define	PCI_SUBCLASS_WIRELESS_IRDA		0x00
#define	PCI_SUBCLASS_WIRELESS_CONSUMERIR	0x01
#define	PCI_SUBCLASS_WIRELESS_RF		0x10
#define	PCI_SUBCLASS_WIRELESS_BLUETOOTH		0x11
#define	PCI_SUBCLASS_WIRELESS_BROADBAND		0x12
#define	PCI_SUBCLASS_WIRELESS_802_11A		0x20
#define	PCI_SUBCLASS_WIRELESS_802_11B		0x21
#define	PCI_SUBCLASS_WIRELESS_MISC		0x80

/* 0x0e I2O (Intelligent I/O) subclasses */
#define	PCI_SUBCLASS_I2O_STANDARD		0x00

/* 0x0f satellite communication subclasses */
/*	PCI_SUBCLASS_SATCOM_???			0x00	/ * XXX ??? */
#define	PCI_SUBCLASS_SATCOM_TV			0x01
#define	PCI_SUBCLASS_SATCOM_AUDIO		0x02
#define	PCI_SUBCLASS_SATCOM_VOICE		0x03
#define	PCI_SUBCLASS_SATCOM_DATA		0x04

/* 0x10 encryption/decryption subclasses */
#define	PCI_SUBCLASS_CRYPTO_NETCOMP		0x00
#define	PCI_SUBCLASS_CRYPTO_ENTERTAINMENT	0x10
#define	PCI_SUBCLASS_CRYPTO_MISC		0x80

/* 0x11 data acquisition and signal processing subclasses */
#define	PCI_SUBCLASS_DASP_DPIO			0x00
#define	PCI_SUBCLASS_DASP_TIMEFREQ		0x01
#define	PCI_SUBCLASS_DASP_SYNC			0x10
#define	PCI_SUBCLASS_DASP_MGMT			0x20
#define	PCI_SUBCLASS_DASP_MISC			0x80

/*
 * PCI BIST/Header Type/Latency Timer/Cache Line Size Register.
 */
#define	PCI_BHLC_REG			0x0c

#define	PCI_BIST_SHIFT				24
#define	PCI_BIST_MASK				0xff
#define	PCI_BIST(bhlcr) \
	    (((bhlcr) >> PCI_BIST_SHIFT) & PCI_BIST_MASK)

#define	PCI_HDRTYPE_SHIFT			16
#define	PCI_HDRTYPE_MASK			0xff
#define	PCI_HDRTYPE(bhlcr) \
	    (((bhlcr) >> PCI_HDRTYPE_SHIFT) & PCI_HDRTYPE_MASK)

#define	PCI_HDRTYPE_TYPE(bhlcr) \
	    (PCI_HDRTYPE(bhlcr) & 0x7f)
#define	PCI_HDRTYPE_MULTIFN(bhlcr) \
	    ((PCI_HDRTYPE(bhlcr) & 0x80) != 0)

#define	PCI_LATTIMER_SHIFT			8
#define	PCI_LATTIMER_MASK			0xff
#define	PCI_LATTIMER(bhlcr) \
	    (((bhlcr) >> PCI_LATTIMER_SHIFT) & PCI_LATTIMER_MASK)

#define	PCI_CACHELINE_SHIFT			0
#define	PCI_CACHELINE_MASK			0xff
#define	PCI_CACHELINE(bhlcr) \
	    (((bhlcr) >> PCI_CACHELINE_SHIFT) & PCI_CACHELINE_MASK)

#define PCI_BHLC_CODE(bist,type,multi,latency,cacheline)		\
	    ((((bist) & PCI_BIST_MASK) << PCI_BIST_SHIFT) |		\
	     (((type) & PCI_HDRTYPE_MASK) << PCI_HDRTYPE_SHIFT) |	\
	     (((multi)?0x80:0) << PCI_HDRTYPE_SHIFT) |			\
	     (((latency) & PCI_LATTIMER_MASK) << PCI_LATTIMER_SHIFT) |	\
	     (((cacheline) & PCI_CACHELINE_MASK) << PCI_CACHELINE_SHIFT))

/*
 * PCI header type
 */
#define PCI_HDRTYPE_DEVICE	0
#define PCI_HDRTYPE_PPB		1
#define PCI_HDRTYPE_PCB		2

/*
 * Mapping registers
 */
#define	PCI_MAPREG_START		0x10
#define	PCI_MAPREG_END			0x28
#define	PCI_MAPREG_ROM			0x30
#define	PCI_MAPREG_PPB_END		0x18
#define	PCI_MAPREG_PCB_END		0x14

#define	PCI_MAPREG_TYPE(mr)						\
	    ((mr) & PCI_MAPREG_TYPE_MASK)
#define	PCI_MAPREG_TYPE_MASK			0x00000001

#define	PCI_MAPREG_TYPE_MEM			0x00000000
#define	PCI_MAPREG_TYPE_IO			0x00000001
#define	PCI_MAPREG_ROM_ENABLE			0x00000001

#define	PCI_MAPREG_MEM_TYPE(mr)						\
	    ((mr) & PCI_MAPREG_MEM_TYPE_MASK)
#define	PCI_MAPREG_MEM_TYPE_MASK		0x00000006

#define	PCI_MAPREG_MEM_TYPE_32BIT		0x00000000
#define	PCI_MAPREG_MEM_TYPE_32BIT_1M		0x00000002
#define	PCI_MAPREG_MEM_TYPE_64BIT		0x00000004

#define	PCI_MAPREG_MEM_PREFETCHABLE(mr)				\
	    (((mr) & PCI_MAPREG_MEM_PREFETCHABLE_MASK) != 0)
#define	PCI_MAPREG_MEM_PREFETCHABLE_MASK	0x00000008

#define	PCI_MAPREG_MEM_ADDR(mr)						\
	    ((mr) & PCI_MAPREG_MEM_ADDR_MASK)
#define	PCI_MAPREG_MEM_SIZE(mr)						\
	    (PCI_MAPREG_MEM_ADDR(mr) & -PCI_MAPREG_MEM_ADDR(mr))
#define	PCI_MAPREG_MEM_ADDR_MASK		0xfffffff0

#define	PCI_MAPREG_MEM64_ADDR(mr)					\
	    ((mr) & PCI_MAPREG_MEM64_ADDR_MASK)
#define	PCI_MAPREG_MEM64_SIZE(mr)					\
	    (PCI_MAPREG_MEM64_ADDR(mr) & -PCI_MAPREG_MEM64_ADDR(mr))
#define	PCI_MAPREG_MEM64_ADDR_MASK		0xfffffffffffffff0ULL

#define	PCI_MAPREG_IO_ADDR(mr)						\
	    ((mr) & PCI_MAPREG_IO_ADDR_MASK)
#define	PCI_MAPREG_IO_SIZE(mr)						\
	    (PCI_MAPREG_IO_ADDR(mr) & -PCI_MAPREG_IO_ADDR(mr))
#define	PCI_MAPREG_IO_ADDR_MASK			0xfffffffc

#define PCI_MAPREG_SIZE_TO_MASK(size)					\
	    (-(size))

#define PCI_MAPREG_NUM(offset)						\
	    (((unsigned)(offset)-PCI_MAPREG_START)/4)


/*
 * Cardbus CIS pointer (PCI rev. 2.1)
 */
#define PCI_CARDBUS_CIS_REG 0x28

/*
 * Subsystem identification register; contains a vendor ID and a device ID.
 * Types/macros for PCI_ID_REG apply.
 * (PCI rev. 2.1)
 */
#define PCI_SUBSYS_ID_REG 0x2c

/*
 * Capabilities link list (PCI rev. 2.2)
 */
#define	PCI_CAPLISTPTR_REG		0x34	/* header type 0 */
#define	PCI_CARDBUS_CAPLISTPTR_REG	0x14	/* header type 2 */
#define	PCI_CAPLIST_PTR(cpr)	((cpr) & 0xff)
#define	PCI_CAPLIST_NEXT(cr)	(((cr) >> 8) & 0xff)
#define	PCI_CAPLIST_CAP(cr)	((cr) & 0xff)

#define	PCI_CAP_RESERVED0	0x00
#define	PCI_CAP_PWRMGMT		0x01
#define	PCI_CAP_AGP		0x02
#define PCI_CAP_AGP_MAJOR(cr)	(((cr) >> 20) & 0xf)
#define PCI_CAP_AGP_MINOR(cr)	(((cr) >> 16) & 0xf)
#define	PCI_CAP_VPD		0x03
#define	PCI_CAP_SLOTID		0x04
#define	PCI_CAP_MSI		0x05
#define	PCI_CAP_CPCI_HOTSWAP	0x06
#define	PCI_CAP_PCIX		0x07
#define	PCI_CAP_LDT		0x08
#define	PCI_CAP_VENDSPEC	0x09
#define	PCI_CAP_DEBUGPORT	0x0a
#define	PCI_CAP_CPCI_RSRCCTL	0x0b
#define	PCI_CAP_HOTPLUG		0x0c
#define	PCI_CAP_AGP8		0x0e
#define	PCI_CAP_SECURE		0x0f
#define	PCI_CAP_PCIEXPRESS     	0x10
#define	PCI_CAP_MSIX		0x11

/*
 * Vital Product Data; access via capability pointer (PCI rev 2.2).
 */
#define	PCI_VPD_ADDRESS_MASK	0x7fff
#define	PCI_VPD_ADDRESS_SHIFT	16
#define	PCI_VPD_ADDRESS(ofs)	\
	(((ofs) & PCI_VPD_ADDRESS_MASK) << PCI_VPD_ADDRESS_SHIFT)
#define	PCI_VPD_DATAREG(ofs)	((ofs) + 4)
#define	PCI_VPD_OPFLAG		0x80000000

#define PCI_MSI_MCR_MMC(cr)     (((cr) >> 17) & 0x7)
#define PCI_MSI_MCR_64BIT       0x00800000

/*
 * Power Management Capability; access via capability pointer.
 */

/* Power Management Capability Register */
#define PCI_PMCR		0x02
#define PCI_PMCR_D1SUPP		0x0200
#define PCI_PMCR_D2SUPP		0x0400
/* Power Management Control Status Register */
#define PCI_PMCSR		0x04
#define PCI_PMCSR_STATE_MASK	0x03
#define PCI_PMCSR_STATE_D0      0x00
#define PCI_PMCSR_STATE_D1      0x01
#define PCI_PMCSR_STATE_D2      0x02
#define PCI_PMCSR_STATE_D3      0x03

/*
 * PCI-X capability.
 */

/*
 * Command. 16 bits at offset 2 (e.g. upper 16 bits of the first 32-bit
 * word at the capability; the lower 16 bits are the capability ID and
 * next capability pointer).
 *
 * Since we always read PCI config space in 32-bit words, we define these
 * as 32-bit values, offset and shifted appropriately.  Make sure you perform
 * the appropriate R/M/W cycles!
 */
#define PCI_PCIX_CMD			0x00
#define PCI_PCIX_CMD_PERR_RECOVER	0x00010000
#define PCI_PCIX_CMD_RELAXED_ORDER	0x00020000
#define PCI_PCIX_CMD_BYTECNT_MASK	0x000c0000
#define	PCI_PCIX_CMD_BYTECNT_SHIFT	18
#define		PCI_PCIX_CMD_BCNT_512		0x00000000
#define		PCI_PCIX_CMD_BCNT_1024		0x00040000
#define		PCI_PCIX_CMD_BCNT_2048		0x00080000
#define		PCI_PCIX_CMD_BCNT_4096		0x000c0000
#define PCI_PCIX_CMD_SPLTRANS_MASK	0x00700000
#define		PCI_PCIX_CMD_SPLTRANS_1		0x00000000
#define		PCI_PCIX_CMD_SPLTRANS_2		0x00100000
#define		PCI_PCIX_CMD_SPLTRANS_3		0x00200000
#define		PCI_PCIX_CMD_SPLTRANS_4		0x00300000
#define		PCI_PCIX_CMD_SPLTRANS_8		0x00400000
#define		PCI_PCIX_CMD_SPLTRANS_12	0x00500000
#define		PCI_PCIX_CMD_SPLTRANS_16	0x00600000
#define		PCI_PCIX_CMD_SPLTRANS_32	0x00700000

/*
 * Status. 32 bits at offset 4.
 */
#define PCI_PCIX_STATUS			0x04
#define PCI_PCIX_STATUS_FN_MASK		0x00000007
#define PCI_PCIX_STATUS_DEV_MASK	0x000000f8
#define PCI_PCIX_STATUS_BUS_MASK	0x0000ff00
#define PCI_PCIX_STATUS_64BIT		0x00010000
#define PCI_PCIX_STATUS_133		0x00020000
#define PCI_PCIX_STATUS_SPLDISC		0x00040000
#define PCI_PCIX_STATUS_SPLUNEX		0x00080000
#define PCI_PCIX_STATUS_DEVCPLX		0x00100000
#define PCI_PCIX_STATUS_MAXB_MASK	0x00600000
#define	PCI_PCIX_STATUS_MAXB_SHIFT	21
#define		PCI_PCIX_STATUS_MAXB_512	0x00000000
#define		PCI_PCIX_STATUS_MAXB_1024	0x00200000
#define		PCI_PCIX_STATUS_MAXB_2048	0x00400000
#define		PCI_PCIX_STATUS_MAXB_4096	0x00600000
#define PCI_PCIX_STATUS_MAXST_MASK	0x03800000
#define		PCI_PCIX_STATUS_MAXST_1		0x00000000
#define		PCI_PCIX_STATUS_MAXST_2		0x00800000
#define		PCI_PCIX_STATUS_MAXST_3		0x01000000
#define		PCI_PCIX_STATUS_MAXST_4		0x01800000
#define		PCI_PCIX_STATUS_MAXST_8		0x02000000
#define		PCI_PCIX_STATUS_MAXST_12	0x02800000
#define		PCI_PCIX_STATUS_MAXST_16	0x03000000
#define		PCI_PCIX_STATUS_MAXST_32	0x03800000
#define PCI_PCIX_STATUS_MAXRS_MASK	0x1c000000
#define		PCI_PCIX_STATUS_MAXRS_1K	0x00000000
#define		PCI_PCIX_STATUS_MAXRS_2K	0x04000000
#define		PCI_PCIX_STATUS_MAXRS_4K	0x08000000
#define		PCI_PCIX_STATUS_MAXRS_8K	0x0c000000
#define		PCI_PCIX_STATUS_MAXRS_16K	0x10000000
#define		PCI_PCIX_STATUS_MAXRS_32K	0x14000000
#define		PCI_PCIX_STATUS_MAXRS_64K	0x18000000
#define		PCI_PCIX_STATUS_MAXRS_128K	0x1c000000
#define PCI_PCIX_STATUS_SCERR			0x20000000


/*
 * Interrupt Configuration Register; contains interrupt pin and line.
 */
#define	PCI_INTERRUPT_REG		0x3c

typedef u8 pci_intr_latency_t;
typedef u8 pci_intr_grant_t;
typedef u8 pci_intr_pin_t;
typedef u8 pci_intr_line_t;

#define PCI_MAX_LAT_SHIFT			24
#define	PCI_MAX_LAT_MASK			0xff
#define	PCI_MAX_LAT(icr) \
	    (((icr) >> PCI_MAX_LAT_SHIFT) & PCI_MAX_LAT_MASK)

#define PCI_MIN_GNT_SHIFT			16
#define	PCI_MIN_GNT_MASK			0xff
#define	PCI_MIN_GNT(icr) \
	    (((icr) >> PCI_MIN_GNT_SHIFT) & PCI_MIN_GNT_MASK)

#define	PCI_INTERRUPT_GRANT_SHIFT		24
#define	PCI_INTERRUPT_GRANT_MASK		0xff
#define	PCI_INTERRUPT_GRANT(icr) \
	    (((icr) >> PCI_INTERRUPT_GRANT_SHIFT) & PCI_INTERRUPT_GRANT_MASK)

#define	PCI_INTERRUPT_LATENCY_SHIFT		16
#define	PCI_INTERRUPT_LATENCY_MASK		0xff
#define	PCI_INTERRUPT_LATENCY(icr) \
	    (((icr) >> PCI_INTERRUPT_LATENCY_SHIFT) & PCI_INTERRUPT_LATENCY_MASK)

#define	PCI_INTERRUPT_PIN_SHIFT			8
#define	PCI_INTERRUPT_PIN_MASK			0xff
#define	PCI_INTERRUPT_PIN(icr) \
	    (((icr) >> PCI_INTERRUPT_PIN_SHIFT) & PCI_INTERRUPT_PIN_MASK)

#define	PCI_INTERRUPT_LINE_SHIFT		0
#define	PCI_INTERRUPT_LINE_MASK			0xff
#define	PCI_INTERRUPT_LINE(icr) \
	    (((icr) >> PCI_INTERRUPT_LINE_SHIFT) & PCI_INTERRUPT_LINE_MASK)

#define PCI_INTERRUPT_CODE(lat,gnt,pin,line)		\
	  ((((lat)&PCI_INTERRUPT_LATENCY_MASK)<<PCI_INTERRUPT_LATENCY_SHIFT)| \
	   (((gnt)&PCI_INTERRUPT_GRANT_MASK)  <<PCI_INTERRUPT_GRANT_SHIFT)  | \
	   (((pin)&PCI_INTERRUPT_PIN_MASK)    <<PCI_INTERRUPT_PIN_SHIFT)    | \
	   (((line)&PCI_INTERRUPT_LINE_MASK)  <<PCI_INTERRUPT_LINE_SHIFT))

#define	PCI_INTERRUPT_PIN_NONE			0x00
#define	PCI_INTERRUPT_PIN_A			0x01
#define	PCI_INTERRUPT_PIN_B			0x02
#define	PCI_INTERRUPT_PIN_C			0x03
#define	PCI_INTERRUPT_PIN_D			0x04
#define	PCI_INTERRUPT_PIN_MAX			0x04

/* Header Type 1 (Bridge) configuration registers */
#define PCI_BRIDGE_BUS_REG		0x18
#define   PCI_BRIDGE_BUS_PRIMARY_SHIFT		0
#define   PCI_BRIDGE_BUS_SECONDARY_SHIFT	8
#define   PCI_BRIDGE_BUS_SUBORDINATE_SHIFT	16

#define PCI_BRIDGE_STATIO_REG		0x1C
#define	  PCI_BRIDGE_STATIO_IOBASE_SHIFT	0
#define	  PCI_BRIDGE_STATIO_IOLIMIT_SHIFT	8
#define	  PCI_BRIDGE_STATIO_STATUS_SHIFT	16
#define	  PCI_BRIDGE_STATIO_IOBASE_MASK		0xf0
#define	  PCI_BRIDGE_STATIO_IOLIMIT_MASK	0xf0
#define	  PCI_BRIDGE_STATIO_STATUS_MASK		0xffff
#define	  PCI_BRIDGE_IO_32BITS(reg)		(((reg) & 0xf) == 1)

#define PCI_BRIDGE_MEMORY_REG		0x20
#define	  PCI_BRIDGE_MEMORY_BASE_SHIFT		4
#define	  PCI_BRIDGE_MEMORY_LIMIT_SHIFT		20
#define	  PCI_BRIDGE_MEMORY_BASE_MASK		0xffff
#define	  PCI_BRIDGE_MEMORY_LIMIT_MASK		0xffff

#define PCI_BRIDGE_PREFETCHMEM_REG	0x24
#define	  PCI_BRIDGE_PREFETCHMEM_BASE_SHIFT	4
#define	  PCI_BRIDGE_PREFETCHMEM_LIMIT_SHIFT	20
#define	  PCI_BRIDGE_PREFETCHMEM_BASE_MASK	0xffff
#define	  PCI_BRIDGE_PREFETCHMEM_LIMIT_MASK	0xffff
#define	  PCI_BRIDGE_PREFETCHMEM_64BITS(reg)	((reg) & 0xf)

#define PCI_BRIDGE_PREFETCHBASE32_REG	0x28
#define PCI_BRIDGE_PREFETCHLIMIT32_REG	0x2C

#define PCI_BRIDGE_IOHIGH_REG		0x30
#define	  PCI_BRIDGE_IOHIGH_BASE_SHIFT		0
#define	  PCI_BRIDGE_IOHIGH_LIMIT_SHIFT		16
#define	  PCI_BRIDGE_IOHIGH_BASE_MASK		0xffff
#define	  PCI_BRIDGE_IOHIGH_LIMIT_MASK		0xffff

#define PCI_BRIDGE_CONTROL_REG		0x3C
#define	  PCI_BRIDGE_CONTROL_SHIFT		16
#define	  PCI_BRIDGE_CONTROL_MASK		0xffff
#define   PCI_BRIDGE_CONTROL_PERE		(1 <<  0)
#define   PCI_BRIDGE_CONTROL_SERR		(1 <<  1)
#define   PCI_BRIDGE_CONTROL_ISA		(1 <<  2)
#define   PCI_BRIDGE_CONTROL_VGA		(1 <<  3)
/* Reserved					(1 <<  4) */
#define   PCI_BRIDGE_CONTROL_MABRT		(1 <<  5)
#define   PCI_BRIDGE_CONTROL_SECBR		(1 <<  6)
#define   PCI_BRIDGE_CONTROL_SECFASTB2B		(1 <<  7)
#define   PCI_BRIDGE_CONTROL_PRI_DISC_TIMER	(1 <<  8)
#define   PCI_BRIDGE_CONTROL_SEC_DISC_TIMER	(1 <<  9)
#define   PCI_BRIDGE_CONTROL_DISC_TIMER_STAT	(1 << 10)
#define   PCI_BRIDGE_CONTROL_DISC_TIMER_SERR	(1 << 11)
/* Reserved					(1 << 12) - (1 << 15) */

/*
 * Vital Product Data resource tags.
 */
struct pci_vpd_smallres {
	u8		vpdres_byte0;		/* length of data + tag */
	/* Actual data. */
} __attribute__((__packed__));

struct pci_vpd_largeres {
	u8		vpdres_byte0;
	u8		vpdres_len_lsb;		/* length of data only */
	u8		vpdres_len_msb;
	/* Actual data. */
} __attribute__((__packed__));

#define	PCI_VPDRES_ISLARGE(x)			((x) & 0x80)

#define	PCI_VPDRES_SMALL_LENGTH(x)		((x) & 0x7)
#define	PCI_VPDRES_SMALL_NAME(x)		(((x) >> 3) & 0xf)

#define	PCI_VPDRES_LARGE_NAME(x)		((x) & 0x7f)

#define	PCI_VPDRES_TYPE_COMPATIBLE_DEVICE_ID	0x3	/* small */
#define	PCI_VPDRES_TYPE_VENDOR_DEFINED		0xe	/* small */
#define	PCI_VPDRES_TYPE_END_TAG			0xf	/* small */

#define	PCI_VPDRES_TYPE_IDENTIFIER_STRING	0x02	/* large */
#define	PCI_VPDRES_TYPE_VPD			0x10	/* large */

struct pci_vpd {
	u8		vpd_key0;
	u8		vpd_key1;
	u8		vpd_len;		/* length of data only */
	/* Actual data. */
} __attribute__((__packed__));

/*
 * Recommended VPD fields:
 *
 *	PN		Part number of assembly
 *	FN		FRU part number
 *	EC		EC level of assembly
 *	MN		Manufacture ID
 *	SN		Serial Number
 *
 * Conditionally recommended VPD fields:
 *
 *	LI		Load ID
 *	RL		ROM Level
 *	RM		Alterable ROM Level
 *	NA		Network Address
 *	DD		Device Driver Level
 *	DG		Diagnostic Level
 *	LL		Loadable Microcode Level
 *	VI		Vendor ID/Device ID
 *	FU		Function Number
 *	SI		Subsystem Vendor ID/Subsystem ID
 *
 * Additional VPD fields:
 *
 *	Z0-ZZ		User/Product Specific
 */

/*
 * Threshold below which 32bit PCI DMA needs bouncing.
 */
#define PCI32_DMA_BOUNCE_THRESHOLD	0x100000000ULL

#endif /* _DEV_PCI_PCIREG_H_ */
