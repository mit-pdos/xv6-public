#pragma once

template<class A, class B>
class pair {
 public:
  typedef A first_type;
  typedef B second_type;

  A first;
  B second;

  pair(const pair&) = default;
  pair(pair&&) = default;
  constexpr pair() : first(), second() {}
  pair(const A &a, const B &b) : first(a), second(b) {}

  bool operator==(const pair<A, B> &other) {
    return first == other.first && second == other.second;
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
make_pair(const A &a, const B &b)
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

class retryable {};

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

  struct ostream { int next_width; };
  extern ostream cout;

  static inline
  ostream& operator<<(ostream &s, const char *str) {
    if (!str)
      str = "(null)";

    int len = strlen(str);
    cprintf("%s", str);
    while (len < s.next_width) {
      cprintf(" ");
      len++;
    }
    s.next_width = 0;
    return s;
  }

  static inline
  ostream& operator<<(ostream &s, u32 v) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", v);
    return s << buf;
  }

  static inline
  ostream& operator<<(ostream &s, u64 v) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%ld", v);
    return s << buf;
  }

  static inline
  ostream& operator<<(ostream &s, ostream& (*xform)(ostream&)) {
    return xform(s);
  }

  static inline ostream& endl(ostream &s) { s << "\n"; return s; }
  static inline ostream& left(ostream &s) { return s; }

  struct ssetw { int _n; };
  static inline ssetw setw(int n) { return { n }; }

  static inline
  ostream& operator<<(ostream &s, const ssetw &sw) {
    s.next_width = sw._n;
    return s;
  }

  class bad_alloc : public retryable {};
}

/* C++ runtime */
void *operator new(unsigned long nbytes);
void *operator new(unsigned long nbytes, void *buf);
void *operator new[](unsigned long nbytes);
void operator delete(void *p);
void operator delete[](void *p);

/* Ref: http://sourcery.mentor.com/public/cxx-abi/abi.html */
extern "C" void __cxa_pure_virtual(void);
extern "C" int  __cxa_guard_acquire(s64 *guard_object);
extern "C" void __cxa_guard_release(s64 *guard_object);
extern "C" void __cxa_guard_abort(s64 *guard_object);
extern "C" int  __cxa_atexit(void (*f)(void *), void *p, void *d);
extern void *__dso_handle;

#define NEW_DELETE_OPS(classname)                                   \
  static void* operator new(unsigned long nbytes) {                 \
    assert(nbytes == sizeof(classname));                            \
    void* p = kmalloc(sizeof(classname), #classname);               \
    if (p == nullptr)                                               \
      throw_bad_alloc();                                            \
    return p;                                                       \
  }                                                                 \
                                                                    \
  static void* operator new(unsigned long nbytes, classname *buf) { \
    assert(nbytes == sizeof(classname));                            \
    return buf;                                                     \
  }                                                                 \
                                                                    \
  static void operator delete(void *p) {                            \
    kmfree(p, sizeof(classname));                                   \
  }

template<class T>
class scoped_cleanup_obj {
 private:
  T handler_;
  bool active_;

 public:
  scoped_cleanup_obj(const T& h) : handler_(h), active_(true) {};
  ~scoped_cleanup_obj() { if (active_) handler_(); }
  void dismiss() { active_ = false; }

  void operator=(const scoped_cleanup_obj&) = delete;
  scoped_cleanup_obj(const scoped_cleanup_obj&) = delete;
  scoped_cleanup_obj(scoped_cleanup_obj&& other) :
    handler_(other.handler_), active_(other.active_) { other.dismiss(); }
};

template<class T>
scoped_cleanup_obj<T>
scoped_cleanup(const T& h)
{
  return scoped_cleanup_obj<T>(h);
}

static void inline
throw_bad_alloc()
{
#if EXCEPTIONS
  throw std::bad_alloc();
#else
  panic("bad alloc");
#endif
}
