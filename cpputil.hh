#pragma once

template<class A, class B>
class pair {
 public:
  A _a;
  B _b;

  pair(const A &a, const B &b) : _a(a), _b(b) {}

  bool operator==(const pair<A, B> &other) {
    return _a == other._a && _b == other._b;
  }
};

template<int N>
class strbuf {
 public:
  char _buf[N];

  strbuf(const char *s) {
    strncpy(_buf, s, N);
  }

  bool operator==(const strbuf<N> &other) {
    return !strncmp(_buf, other._buf, N);
  }
};

template<class A, class B>
pair<A, B>
mkpair(const A &a, const B &b)
{
  return pair<A, B>(a, b);
}

/* C++ runtime */
void *operator new(unsigned long nbytes);
void *operator new(unsigned long nbytes, void *buf);
void operator delete(void *p);
extern "C" void __cxa_pure_virtual(void);

