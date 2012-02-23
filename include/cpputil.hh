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

class scoped_acquire {
 private:
  spinlock *_l;

 public:
  scoped_acquire(spinlock *l) : _l(0) { acquire(l); }
  ~scoped_acquire() { release(); }
  void release() { if (_l) { ::release(_l); _l = 0; } }
  void acquire(spinlock *l) { assert(!_l); ::acquire(l); _l = l; }
};

namespace std {
  template<class T>
  struct remove_reference
  { typedef T type; };

  template<class T>
  struct remove_reference<T&>
  { typedef T type; };

  template<class T>
  struct remove_reference<T&&>
  { typedef T type; };

  template<class T>
  typename remove_reference<T>::type&&
  move(T&& a)
  {
    return static_cast<typename remove_reference<T>::type&&>(a);
  }
}

/* C++ runtime */
// void *operator new(unsigned long nbytes);
// void *operator new(unsigned long nbytes, void *buf);
void *operator new[](unsigned long nbytes);
// void operator delete(void *p);
void operator delete[](void *p);
extern "C" void __cxa_pure_virtual(void);

#define NEW_DELETE_OPS(classname)                           \
  static void* operator new(unsigned long nbytes) {         \
    assert(nbytes == sizeof(classname));                    \
    return kmalloc(sizeof(classname));                      \
  }                                                         \
                                                            \
  static void operator delete(void *p) {                    \
    return kmfree(p, sizeof(classname));                    \
  }

