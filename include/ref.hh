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

  bool init(T* p) {
    if (ptr_ || !p->tryinc())
      return false;
    ptr_ = p;
    return true;
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

class referenced {
public:
  // Start with 1 reference
  referenced() { ref_.v = 0; }

  // The number of valid references is:
  //   ref_.invalid ? 0 : ref_.count+1;

  inline bool valid() const {
    return ref_.invalid == 0;
  }

  inline void inc() const {
    // If references is 0 (i.e. ref_.count is 0xffffffff) a 32-bit 
    // increment will increases ref_.count to 0, but ref_.invalid
    // will remain unchanged.
    asm volatile("lock; incl %0" : "+m" (ref_.count));
  }

  inline bool tryinc() const {
    inc();
    return valid();
  }

  inline void dec() const {
    unsigned char c;
    // If references is 1 (i.e. ref_.v is 0), a 64-bit decrement will
    // underflow ref_.invalid to 0xffffffff (and ref_.count to 0xffffffff).
    asm volatile("lock; decq %0; sets %1" : "+m" (ref_.v), "=qm" (c));
    if (c)
      onzero();
  }
  
  NEW_DELETE_OPS(referenced);

protected:
  virtual ~referenced() { }
  virtual void onzero() const { delete this; }

private:
  mutable union {
    volatile u64 v;
    struct {
      volatile u32 count;
      volatile u32 invalid;
    };
  } ref_;
};
