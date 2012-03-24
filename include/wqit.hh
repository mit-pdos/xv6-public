template<typename T>
struct wq_num
{
  wq_num(T v) { v_ = v; }

  bool operator!= (const wq_num<T> &i) const { return v_ != i.v_; }
  bool operator!= (const T &v) const         { return v_ != v; }
  bool operator<  (const wq_num<T> &i) const { return v_ < i.v_; }
  bool operator<  (const T &v) const         { return v_ < v; }

  T copy_value() {
    return v_;
  }

  wq_num<T>& operator ++() {
    ++v_;
    return *this;
  }

  T v_;
};
