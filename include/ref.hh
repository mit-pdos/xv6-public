#include "atomic.hh"

template <class T>
class sref {
public:
  sref(T* p = nullptr) : ptr_(p) {
    if (ptr_)
      ptr_->inc();
  }

  ~sref() {
    if (ptr_)
      ptr_->dec();
  }

  void init(T* p) {
    const T* save = ptr_;
    ptr_ = p;
    if (ptr_)
      ptr_->inc();
    if (save)
      save->dec();
  }

  bool operator==(const sref<T>& pr) const { return ptr_ == pr.ptr_; }
  bool operator!=(const sref<T>& pr) const { return ptr_ != pr.ptr_; }
  bool operator==(T* p) const { return ptr_ == p; }
  bool operator!=(T* p) const { return ptr_ != p; }

  const T * operator->() const { return ptr_; }
  T * operator->() { return ptr_; }
  T * ptr() const { return ptr_; }

private:
  sref<T>& operator=(sref<T>& mp);
  sref<T>& operator=(T* p);
  sref<T>& operator=(const sref<T>& pr);
  sref(const sref<T>& pr);

  T *ptr_;
};

template <class T>
class lref {
public:
  lref(T* p = nullptr) : ptr_(p) {
    if (ptr_)
      ptr_->inc();
  }

  lref(const lref<T>& pr) : ptr_(pr.ptr_) {
    if (ptr_)
      ptr_->inc();
  }

  ~lref() {
    if (ptr_)
      ptr_->dec();
  }

  bool operator==(const lref<T>& pr) const { return ptr_ == pr.ptr_; }
  bool operator!=(const lref<T>& pr) const { return ptr_ != pr.ptr_; }
  bool operator==(T* p) const { return ptr_ == p; }
  bool operator!=(T* p) const { return ptr_ != p; }

  const T * operator->() const { return ptr_; }
  T * operator->() { return ptr_; }
  T * ptr() const { return ptr_; }

  lref<T>& operator=(const lref<T>& pr) {
    const T* save = ptr_;
    ptr_ = pr.ptr_; 
    if (ptr_)
      ptr_->inc();
    if (save)
      save->dec();
    return *this;
  }

private:
  lref<T>& operator=( lref<T>& mp );
  lref<T>& operator=( T* p );

  T *ptr_;
};

class referenced {
public:
  referenced() : ref_(0) {}

  u64 ref() const { 
    return ref_;
  }

  inline const referenced* inc() const { 
    ++ref_;
    return this;
  }

  inline void dec() const {
    if (--ref_ == 0)
      onzero();
  }
  
  NEW_DELETE_OPS(referenced);

protected:
  virtual ~referenced() { }
  virtual void onzero() const { delete this; }

private:
    mutable std::atomic<u64> ref_;
};
