template <typename T>
struct percpu {
  int myid() {
    return mycpu()->id;
  }

  const T* operator->() const {
    return cpu(myid());
  }

  T* operator->() {
    return cpu(myid());
  }

  T& operator*() {
    return *cpu(myid());
  }

  T& operator[](int id) { 
    return *cpu(id);
  }

  const T& operator[](int id) const { 
    return *cpu(id);
  }

  T* cpu(int id) {
    return &pad_[id].v_;
  }

  struct {
    T v_ __mpalign__;
    __padout__;
  } pad_[NCPU];
};
