#pragma once

template<class T>
class markptr_ptr;

template<class T>
class markptr_mark;

template<class T>
class markptr {
 protected:
  std::atomic<uptr> _p;

 public:
  markptr() : _p(0) {}
  markptr(T* v) : _p((uptr) v) {}
  markptr(const markptr<T> &v) : _p(v._p.load()) {}

  void operator=(T* v) { _p = (uptr) v; }
  void operator=(const markptr<T> &v) { _p = v._p.load(); }

  bool operator!=(const markptr<T> &v) const { return _p != v._p; }
  bool operator==(const markptr<T> &v) const { return _p == v._p; }

  markptr_ptr<T>& ptr() {
    return *(markptr_ptr<T>*) this;
  }
  const markptr_ptr<T>& ptr() const {
    return *(const markptr_ptr<T>*) this;
  }

  markptr_mark<T>& mark() {
    return *(markptr_mark<T>*) this;
  }
  const markptr_mark<T>& mark() const {
    return *(const markptr_mark<T>*) this;
  }

  // Convenience operator to avoid having to write out xx.ptr()->...
  T* operator->() { return ptr(); }

  bool cmpxch(markptr<T> expected, markptr<T> desired) {
    uptr ee = expected._p.load();
    return _p.compare_exchange_weak(ee, desired._p.load());
  }
};

template<class T>
class markptr_ptr : private markptr<T> {
 public:
  void operator=(T* p) {
    uptr p0, p1;
    do {
      p0 = markptr<T>::_p.load();
      p1 = (p0 & 1) | (uptr) p;
    } while (!markptr<T>::_p.compare_exchange_weak(p0, p1));
  }

  bool cmpxch_update(T** expected, T* desired) {
    uptr p0, p1;
    do {
      p0 = markptr<T>::_p.load();
      p1 = (p0 & 1) | (uptr) desired;

      T* cur = (T*) (p0 & ~1);
      if (cur != *expected) {
        *expected = cur;
        return false;
      }
    } while (!markptr<T>::_p.compare_exchange_weak(p0, p1));

    return true;
  }

  T* load() const {
    return (T*) (markptr<T>::_p.load() & ~1);
  }

  operator T*() const { return load(); }
};

template<class T>
class markptr_mark : public markptr<T> {
 public:
  void operator=(bool m) {
    xchg(m);
  }

  bool xchg(bool m) {
    uptr p0, p1;
    do {
      p0 = markptr<T>::_p.load();
      p1 = (p0 & ~1) | !!m;
    } while (!markptr<T>::_p.compare_exchange_weak(p0, p1));
    return p0 & 1;
  }

  bool load() const {
    return markptr<T>::_p.load() & 1;
  }

  operator bool() const { return load(); }
};
