#pragma once

// Extensible, type-safe, usable print streams.
//
// External users of this API should call the print or println methods
// of a print_stream.  To specially format numbers, use sfmt or shex.
// To output byte buffers, use sbuf.
//
// Extensions of this API to add printable types should be implemented
// as overloads of to_stream and should call other to_stream
// functions.
//
// Output stream types should be implemented as subclasses of
// print_stream.

#include <utility>

struct sbuf
{
  const char *base;
  size_t len;

  sbuf(const char *b, size_t l) : base(b), len(l) { }
  sbuf(const sbuf &o) = default;
  sbuf(sbuf &&o) = default;
};

class print_stream
{
public:
  // By making this constexpr (and assuming derived classes have
  // either no constructor or a constexpr constructor), print_streams
  // can be fully initialized at compile time, including the vtable
  // pointer (meaning global print_streams don't require static
  // construction).
  constexpr print_stream() : enabled(true) { }
  virtual ~print_stream() { }

  // Write each of the arguments to this stream in order.
  template<typename... T>
  void print(T&&... args)
  {
    if (enabled)
      _print(std::forward<T>(args)...);
  }

  // Like print, but append a newline.
  template<typename... T>
  void println(T&&... args)
  {
    if (enabled) {
      _print(std::forward<T>(args)...);
      write('\n');
    }
  }

protected:
  bool enabled;

  constexpr print_stream(bool enabled) : enabled(enabled) { }

  virtual void write(char c)
  {
    write(sbuf(&c, 1));
  }

  virtual void write(sbuf buf) = 0;

  friend void to_stream(print_stream *s, char c);
  friend void to_stream(print_stream *s, sbuf b);

private:
  template<typename T1, typename... T>
  void _print(T1 &&arg1, T&&... rest)
  {
    to_stream(this, std::forward<T1>(arg1));
    _print(std::forward<T>(rest)...);
  }

  void _print() { }
};

class null_stream : public print_stream
{
public:
  constexpr null_stream() : print_stream(false) { }

protected:
  void write(char c) { }
  void write(sbuf buf) { }
};

inline
void to_stream(print_stream *s, char c)
{
  s->write(c);
}

inline
void to_stream(print_stream *s, sbuf b)
{
  s->write(b);
}

void to_stream(print_stream *s, int v);
void to_stream(print_stream *s, unsigned v);
void to_stream(print_stream *s, long v);
void to_stream(print_stream *s, unsigned long v);
void to_stream(print_stream *s, long long v);
void to_stream(print_stream *s, unsigned long long v);

void to_stream(print_stream *s, const char *str);
void to_stream(print_stream *s, void *ptr);

class integer_formatter
{
  unsigned long long val_;
  int width_;
  unsigned char base_;
  char pad_;
  bool neg_;
  bool alt_;

  friend void to_stream(print_stream *s, const integer_formatter &n);
public:
  integer_formatter(unsigned long long v, bool neg)
    : val_(v), width_(0), base_(10), pad_(' '), neg_(neg), alt_(false) { }

  integer_formatter &width(int width)
  {
    width_ = width;
    return *this;
  }

  integer_formatter &base(unsigned base)
  {
    if (base == 0 || base > 16)
      base = 10;
    base_ = base;
    return *this;
  }

  integer_formatter &pad(char pad)
  {
    pad_ = pad;
    return *this;
  }

  // Format the number using an alternate form.  If base is 8 and the
  // number is non-zero, this will prefix the output with "0".  If
  // base is 16 and the number of non-zero, this will prefix the
  // output with "0x".
  integer_formatter &alt(bool alt = true)
  {
    alt_ = alt;
    return *this;
  }
};

void to_stream(print_stream *s, const integer_formatter &n);

// Format any integral value.  The default formatting is equivalent to
// passing the value directly to the print stream, but can be
// manipulated using the methods of integer_formatter.
template<typename T>
integer_formatter sfmt(T v)
{
  bool neg = v < 0;
  if (neg)
    v = -v;
  return integer_formatter(v, neg);
}

// Format v in hexadecimal and, if non-zero, preceded by an 0x.
template<typename T>
integer_formatter shex(T v)
{
  return sfmt(v).base(16).alt();
}
