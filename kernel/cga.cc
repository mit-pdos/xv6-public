#include "types.h"
#include "kernel.hh"
#include "amd64.h"

#define BACKSPACE 0x100
#define CRTPORT 0x3d4
static volatile u16 *crt = (u16*)(KBASE + 0xb8000);  // CGA memory
// Black background, (high intensity) white foreground
static int color = ((0 << 4) | 15) << 8;

void
cgaputc(int c)
{
  int pos;
  
  // Cursor position: col + 80*row.
  outb(CRTPORT, 14);
  pos = inb(CRTPORT+1) << 8;
  outb(CRTPORT, 15);
  pos |= inb(CRTPORT+1);

  if(c == '\n')
    pos += 80 - pos%80;
  else if(c == BACKSPACE){
    if(pos > 0) --pos;
  } else
    crt[pos++] = (c&0xff) | color;
  
  if((pos/80) >= 24){  // Scroll up.
    memmove((void *)crt, (void*)(crt+80), sizeof(crt[0])*23*80);
    pos -= 80;
    memset((void *)(crt+pos), 0, sizeof(crt[0])*(24*80 - pos));
  }
  
  outb(CRTPORT, 14);
  outb(CRTPORT+1, pos>>8);
  outb(CRTPORT, 15);
  outb(CRTPORT+1, pos);
  crt[pos] = ' ' | 0x0700;
}

void
initcga(void)
{
  int i;

  for (i = 0; i < 80*25; i++)
    crt[i] = 0x0f20;

  outb(CRTPORT, 14);
  outb(CRTPORT+1, 0);
  outb(CRTPORT, 15);
  outb(CRTPORT+1, 0);

  // Announce that we're here.
  for (const char *p=DEBUG?"xv6 DEBUG\n":"xv6\n"; *p; p++)
    cgaputc(*p);
}
