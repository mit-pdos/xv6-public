#pragma once

extern int mycpuid(void);

template <typename T>
struct percpu {
  const T* operator->() const {
    return cpu(mycpuid());
  }

  T* operator->() {
    return cpu(mycpuid());
  }

  T& operator*() {
    return *cpu(mycpuid());
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
