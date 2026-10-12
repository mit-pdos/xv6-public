#include "types.h"
#include "defs.h"
#include "mmu.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "buf.h"
#include "swap.h"

struct {
  struct spinlock lock;
  uchar map[SWAP_PAGES];
} swapmap;

void
swapinit()
{
  initlock(&swapmap.lock, "swapmap");
}

uint
swap_increase_refcount(uint slot)
{
  acquire(&swapmap.lock);
  if(swapmap.map[--slot] == MAX_REFCOUNT) {
    release(&swapmap.lock);
    return 0;
  }
  swapmap.map[slot]++;
  release(&swapmap.lock);
  return 1;
}

void
swap_decrease_refcount(uint slot)
{
  acquire(&swapmap.lock);
  swapmap.map[--slot]--;
  release(&swapmap.lock);
}

// Return offset + 1
// Just to avoid negative integers
static uint
find_free_swapslot(void)
{
  acquire(&swapmap.lock);
  for(int i = 0; i < SWAP_PAGES; i++) {
    if(swapmap.map[i] == 0) {
      swapmap.map[i]++;
      release(&swapmap.lock);
      return i + 1;
    }
  }
  release(&swapmap.lock);
  return 0;
}

void
read_from_swap(uint page_index, char *dst)
{
	page_index--;
  uint sector_start = page_index * 8;
  for(int i = 0; i < 8; i++){
      struct buf *b = bread(2, sector_start + i);
      memmove(dst + (i * 512), b->data, 512);
      brelse(b);
  }
  acquire(&swapmap.lock);
	swapmap.map[page_index]--;
	release(&swapmap.lock);
}

uint
write_to_swap(char *page)
{
  uint page_index=find_free_swapslot();
  if(!page_index)
    return 0;
  uint sector_start = (page_index-1)*8;

  for(int i = 0; i < 8; i++){
    struct buf *b = bread(2,sector_start + i);
    memmove(b->data, page + (i * 512), 512);
    bwrite(b);
    brelse(b);
  }
  return page_index;
}
