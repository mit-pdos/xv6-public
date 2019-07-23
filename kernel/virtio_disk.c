//
// driver for qemu's virtio disk device.
// uses qemu's mmio interface to virtio.
// qemu presents a "legacy" virtio interface.
//
// qemu ... -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0
//

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "buf.h"
#include "virtio.h"

// the address of virtio mmio register r.
#define R(r) ((volatile uint32 *)(VIRTIO0 + (r)))

struct spinlock vdisk_lock;

// memory for virtio descriptors &c for queue 0.
// this is a global instead of allocated because it has
// to be multiple contiguous pages, which kalloc()
// doesn't support.
__attribute__ ((aligned (PGSIZE)))
static char pages[2*PGSIZE];
static struct VRingDesc *desc;
static uint16 *avail;
static struct UsedArea *used;

// our own book-keeping.
static char free[NUM];  // is a descriptor free?
static uint16 used_idx; // we've looked this far in used[2..NUM].

// track info about in-flight operations,
// for use when completion interrupt arrives.
// indexed by first descriptor index of chain.
static struct {
  struct buf *b;
  char status;
} info[NUM];

void
virtio_disk_init(void)
{
  uint32 status = 0;

  initlock(&vdisk_lock, "virtio_disk");

  if(*R(VIRTIO_MMIO_MAGIC_VALUE) != 0x74726976 ||
     *R(VIRTIO_MMIO_VERSION) != 1 ||
     *R(VIRTIO_MMIO_DEVICE_ID) != 2 ||
     *R(VIRTIO_MMIO_VENDOR_ID) != 0x554d4551){
    panic("could not find virtio disk");
  }
  
  status |= VIRTIO_CONFIG_S_ACKNOWLEDGE;
  *R(VIRTIO_MMIO_STATUS) = status;

  status |= VIRTIO_CONFIG_S_DRIVER;
  *R(VIRTIO_MMIO_STATUS) = status;

  // negotiate features
  uint64 features = *R(VIRTIO_MMIO_DEVICE_FEATURES);
  features &= ~(1 << VIRTIO_BLK_F_RO);
  features &= ~(1 << VIRTIO_BLK_F_SCSI);
  features &= ~(1 << VIRTIO_BLK_F_CONFIG_WCE);
  features &= ~(1 << VIRTIO_BLK_F_MQ);
  features &= ~(1 << VIRTIO_F_ANY_LAYOUT);
  features &= ~(1 << VIRTIO_RING_F_EVENT_IDX);
  features &= ~(1 << VIRTIO_RING_F_INDIRECT_DESC);
  *R(VIRTIO_MMIO_DRIVER_FEATURES) = features;

  // tell device that feature negotiation is complete.
  status |= VIRTIO_CONFIG_S_FEATURES_OK;
  *R(VIRTIO_MMIO_STATUS) = status;

  // tell device we're completely ready.
  status |= VIRTIO_CONFIG_S_DRIVER_OK;
  *R(VIRTIO_MMIO_STATUS) = status;

  *R(VIRTIO_MMIO_GUEST_PAGE_SIZE) = PGSIZE;

  // initialize queue 0.
  *R(VIRTIO_MMIO_QUEUE_SEL) = 0;
  uint32 max = *R(VIRTIO_MMIO_QUEUE_NUM_MAX);
  if(max == 0)
    panic("virtio disk has no queue 0");
  if(max < NUM)
    panic("virtio disk max queue too short");
  *R(VIRTIO_MMIO_QUEUE_NUM) = NUM;
  memset(pages, 0, sizeof(pages));
  *R(VIRTIO_MMIO_QUEUE_PFN) = ((uint64)pages) >> PGSHIFT;

  // desc = pages -- num * VRingDesc
  // avail = pages + 0x40 -- 2 * uint16, then num * uint16
  // used = pages + 4096 -- 2 * uint16, then num * vRingUsedElem

  desc = (struct VRingDesc *) pages;
  avail = (uint16*)(((char*)desc) + NUM*sizeof(struct VRingDesc));
  used = (struct UsedArea *) (pages + PGSIZE);

  for(int i = 0; i < NUM; i++)
    free[i] = 1;

  // plic.c and trap.c arrange for interrupts from VIRTIO0_IRQ.
}

// find a free descriptor, mark it non-free, return its index.
static int
alloc_desc()
{
  for(int i = 0; i < NUM; i++){
    if(free[i]){
      free[i] = 0;
      return i;
    }
  }
  return -1;
}

// mark a descriptor as free.
static void
free_desc(int i)
{
  if(i >= NUM)
    panic("virtio_disk_intr 1");
  if(free[i])
    panic("virtio_disk_intr 2");
  desc[i].addr = 0;
  free[i] = 1;
  wakeup(&free[0]);
}

// free a chain of descriptors.
static void
free_chain(int i)
{
  while(1){
    free_desc(i);
    if(desc[i].flags & VRING_DESC_F_NEXT)
      i = desc[i].next;
    else
      break;
  }
}

static int
alloc3_desc(int *idx)
{
  for(int i = 0; i < 3; i++){
    idx[i] = alloc_desc();
    if(idx[i] < 0){
      for(int j = 0; j < i; j++)
        free_desc(idx[j]);
      return -1;
    }
  }
  return 0;
}

void
virtio_disk_rw(struct buf *b)
{
  uint64 sector = b->blockno * (BSIZE / 512);

  acquire(&vdisk_lock);

  // the spec says that legacy block operations use three
  // descriptors: one for type/reserved/sector, one for
  // the data, one for a 1-byte status result.

  // allocate the three descriptors.
  int idx[3];
  while(1){
    if(alloc3_desc(idx) == 0) {
      break;
    }
    sleep(&free[0], &vdisk_lock);
  }
  
  // format the three descriptors.
  // qemu's virtio-blk.c reads them.

  struct virtio_blk_outhdr {
    uint32 type;
    uint32 reserved;
    uint64 sector;
  } buf0;

  if(b->flags & B_DIRTY)
    buf0.type = VIRTIO_BLK_T_OUT; // write the disk
  else
    buf0.type = VIRTIO_BLK_T_IN; // read the disk
  buf0.reserved = 0;
  buf0.sector = sector;

  // buf0 is on a kernel stack, which is not direct mapped,
  // thus the call to kernelpa().
  desc[idx[0]].addr = (uint64) kernelpa((uint64) &buf0);
  desc[idx[0]].len = sizeof(buf0);
  desc[idx[0]].flags = VRING_DESC_F_NEXT;
  desc[idx[0]].next = idx[1];

  desc[idx[1]].addr = (uint64) b->data;
  desc[idx[1]].len = BSIZE;
  if(b->flags & B_DIRTY)
    desc[idx[1]].flags = 0; // device reads b->data
  else
    desc[idx[1]].flags = VRING_DESC_F_WRITE; // device writes b->data
  desc[idx[1]].flags |= VRING_DESC_F_NEXT;
  desc[idx[1]].next = idx[2];

  info[idx[0]].status = 0;
  desc[idx[2]].addr = (uint64) &info[idx[0]].status;
  desc[idx[2]].len = 1;
  desc[idx[2]].flags = VRING_DESC_F_WRITE; // device writes the status
  desc[idx[2]].next = 0;

  // record struct buf for virtio_disk_intr().
  info[idx[0]].b = b;

  // avail[0] is flags
  // avail[1] tells the device how far to look in avail[2...].
  // avail[2...] are desc[] indices the device should process.
  // we only tell device the first index in our chain of descriptors.
  avail[2 + (avail[1] % NUM)] = idx[0];
  __sync_synchronize();
  avail[1] = avail[1] + 1;

  *R(VIRTIO_MMIO_QUEUE_NOTIFY) = 0; // value is queue number

  // Wait for virtio_disk_intr() to say request has finished.
  while((b->flags & (B_VALID|B_DIRTY)) != B_VALID){
    sleep(b, &vdisk_lock);
  }

  release(&vdisk_lock);
}

void
virtio_disk_intr()
{
  acquire(&vdisk_lock);

  while((used_idx % NUM) != (used->id % NUM)){
    int id = used->elems[used_idx].id;

    if(info[id].status != 0)
      panic("virtio_disk_intr status");

    info[id].b->flags |= B_VALID;
    info[id].b->flags &= ~B_DIRTY;

    wakeup(info[id].b);

    info[id].b = 0;
    free_chain(id);

    used_idx = (used_idx + 1) % NUM;
  }

  release(&vdisk_lock);
}
