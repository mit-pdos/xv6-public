#include "pstream.hh"
#include "user.h"               // For strlen

static void
streamnum (print_stream *s, unsigned long long num,
           bool neg = false, unsigned base = 10, int width = 0, char pad = 0,
           bool alt = false)
{
  char buf[68], *x = buf + sizeof(buf);

  if (num == 0)
    *--x = '0';
  else {
    for (; num; num /= base)
      *--x = "0123456789abcdef"[num % base];
    if (alt) {
      if (base == 16) {
        *--x = 'x';
        *--x = '0';
      } else if (base == 8) {
        *--x = '0';
      }
    }
    if (neg)
      *--x = '-';
  }

  size_t len = buf + sizeof(buf) - x;

  for (; width > len; width--)
    to_stream(s, pad);
  to_stream(s, sbuf(x, len));
  for (; width < 0; width++)
    to_stream(s, pad);
}

#define INT_TO_STREAM(typ)                              \
  void to_stream(print_stream *s, typ v)                \
  {                                                     \
    streamnum(s, v);                                    \
  }                                                     \
                                                        \
  void to_stream(print_stream *s, unsigned typ v)       \
  {                                                     \
    if (v < 0)                                          \
      streamnum(s, -v, true);                           \
    else                                                \
      streamnum(s, v, false);                           \
  }                                                     \
  static_assert(true, "need a semicolon")

INT_TO_STREAM(int);
INT_TO_STREAM(long);
INT_TO_STREAM(long long);

void to_stream(print_stream *s, const char *str)
{
  to_stream(s, sbuf(str, strlen(str)));
}

void to_stream(print_stream *s, void *ptr)
{
  to_stream(s, sbuf("0x", 2));
  streamnum(s, (unsigned long long)ptr, false, 16);
}

void to_stream(print_stream *s, const integer_formatter &n)
{
  streamnum(s, n.val_, n.neg_, n.base_, n.width_, n.pad_, n.alt_);
}
