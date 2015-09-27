// This file contains definitions for IDE/ATA disk controllers.

// IDE sectors are 512 bytes long.
#define SECTOR_SIZE 512

// Base addresses in I/O space for the first two IDE channels,
// each of which supports up to two drives.
#define IDE_DATA_PRIMARY   0x1f0
#define IDE_CTRL_PRIMARY   0x3f4
#define IDE_DATA_SECONDARY 0x170
#define IDE_CTRL_SECONDARY 0x374

// DATA registers for each IDE channel; add offsets to relevant
// DATA base address.
#define IDE_REG_DATA    0x00 // read/write data from/to disk
#define IDE_REG_ERROR   0x01
#define IDE_REG_SECTORS 0x02
#define IDE_REG_LBA0    0x03
#define IDE_REG_LBA1    0x04
#define IDE_REG_LBA2    0x05
#define IDE_REG_DISK    0x06
#define IDE_REG_COMMAND 0x07 // write commands
#define IDE_REG_STATUS  0x07 // read status

// Masks for IDE_REG_DISK.
#define IDE_DISK_CHS 0xA0 // bits 7 and 5 always set, bit 6 LBA clear
#define IDE_DISK_LBA 0xE0 // bits 7 and 5 always set, bit 6 LBA set

// Flags read from IDE_REG_STATUS.
#define IDE_BSY  0x80 // drive busy (with a command?)
#define IDE_DRDY 0x40 // drive ready (for comment? spun up?)
#define IDE_DF   0x20 // drive (write?) failure
#define IDE_DRQ  0x08 // PIO data ready
#define IDE_ERR  0x01 // error, details in IDE_REG_ERROR

// Commands written to IDE_REG_COMMAND. These are
// the PIO read/write commands for LBA-28.
#define IDE_CMD_READ  0x20
#define IDE_CMD_WRITE 0x30
#define IDE_CMD_IDENT 0xEC

// CTRL registers for each IDE channel; add offsets
// to relevant CTRL base address.
#define IDE_REG_CTRL      0x02 // write device control
#define IDE_REG_ALTSTATUS 0x02 // read status (without IRQ stuff)
