#include "types.h"
#include "kernel.h"

static volatile u16 *screen = (u16*)(KBASE+0xb8000);
// Black background, (high intensity) white foreground
static int color = ((0 << 4) | 15) << 8;
static int cursor;

extern void setcursor(int);

static void
cgaputs(const char *s)
{
  uint8 *p, *ep;

  p = (uint8*)s;
  ep = p+strlen(s);

  for (; p < ep; p++)
    cgaputc(*p);
}

void
cgaputc(char c)
{
  int i;

  if(c == '\n') {
    cursor += 80 - cursor%80;
  } else {
    screen[cursor] = color | c;
    cursor++;
  }
  if(cursor == 25*80) {
    memmove((void*)screen+80*2, (void*)screen+80*3, 80*(25-3)*2);
    for(i=0; i<80; i++)
      screen[(24*80+i)] = color | (' ' & 0xff);
    cursor -= 80;
  }
}

void
initcga(void)
{
  // Assume boot.S set up the screen.
  // We're just taking over the cursor position.
  cursor = 2*80;
  setcursor(cursor);
  
  cgaputs("cga...\n");
  setcursor(cursor);
}
