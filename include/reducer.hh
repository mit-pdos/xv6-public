#include "percpu.hh"

template <typename T>
struct reducer_opadd
{
  reducer_opadd(T v) {
    for (int i = 0; i < NCPU; i++)
      v_[i] = 0;
    *v_ = v;
  }

  T operator+=(T i) {
    (*v_) += i;
    return *v_;
  }

  T get_value() {
    T t = 0;
    for (int i = 0; i < NCPU; i++)
      t += v_[i];
    return t;
  }

  percpu<T> v_;
};
