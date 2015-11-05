#include "types.h"
#include "defs.h"
#include "date.h"
#include "x86.h"
#include "cmos.h"

uint cmosread(uint reg)
{
  if(reg >= CMOS_NMI_BIT)
    panic("cmosread: invalid register");

  outb(CMOS_PORT, reg);
  microdelay(200);
  return inb(CMOS_RETURN);
}

void cmoswrite(uint reg, uint data)
{
  if(reg >= CMOS_NMI_BIT)
    panic("cmoswrite: invalid register");
  if(data > 0xff)
    panic("cmoswrite: invalid data");

  outb(CMOS_PORT,  reg);
  microdelay(200);
  outb(CMOS_RETURN, data);
}

static void fill_rtcdate(struct rtcdate *r)
{
  r->second = cmosread(CMOS_SECS);
  r->minute = cmosread(CMOS_MINS);
  r->hour   = cmosread(CMOS_HOURS);
  r->day    = cmosread(CMOS_DAY);
  r->month  = cmosread(CMOS_MONTH);
  r->year   = cmosread(CMOS_YEAR);
}

// qemu seems to use 24-hour GWT and the values are BCD encoded
void cmostime(struct rtcdate *r)
{
  struct rtcdate t1, t2;
  int sb, bcd, tf;

  sb = cmosread(CMOS_STATB);

  bcd = (sb & CMOS_BINARY_BIT) == 0;
  tf = (sb & CMOS_24H_BIT) != 0;

  // make sure CMOS doesn't modify time while we read it
  for(;;){
    fill_rtcdate(&t1);
    if(cmosread(CMOS_STATA) & CMOS_UIP_BIT)
      continue;
    fill_rtcdate(&t2);
    if(memcmp(&t1, &t2, sizeof(t1)) == 0)
      break;
  }

  // backup raw values since BCD conversion removes PM bit from hour
  t2 = t1;

  // convert t1 from BCD
  if(bcd){
#define    CONV(x)     (t1.x = ((t1.x >> 4) * 10) + (t1.x & 0xf))
    CONV(second);
    CONV(minute);
    CONV(hour  );
    CONV(day   );
    CONV(month );
    CONV(year  );
#undef     CONV
  }

  // convert 12 hour format to 24 hour format
  if(!tf){
    if(t2.hour & CMOS_PM_BIT){
      t1.hour = (t1.hour + 12) % 24;
    }
  }

  *r = t1;
  r->year += 2000;
}
