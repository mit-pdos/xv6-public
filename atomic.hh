#pragma once

#define _GLIBCXX_VISIBILITY(x)
#define _GLIBCXX_BEGIN_NAMESPACE_VERSION
#define _GLIBCXX_END_NAMESPACE_VERSION
#define _GLIBCXX_BEGIN_EXTERN_C extern "C" {
#define _GLIBCXX_END_EXTERN_C }
#define __glibcxx_assert(x)

#include "atomic_base.h"
#include "atomic_2.h"

template<class T>
struct atomic : public std::__atomic2::__atomic_base<T> {
  atomic() = default;
  ~atomic() = default;
  atomic(const atomic&) = delete;
  atomic& operator=(const atomic&) = delete;
  atomic& operator=(const atomic&) volatile = delete;

  constexpr atomic(T v) : std::__atomic2::__atomic_base<T>(v) {}

  using std::__atomic2::__atomic_base<T>::operator T;
  using std::__atomic2::__atomic_base<T>::operator=;
};
