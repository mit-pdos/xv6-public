#pragma once

template<class T>
bool
cmpxch(std::atomic<T> *a, T expected, T desired)
{
  return a->compare_exchange_weak(expected, desired);
}

template<class T>
bool
cmpxch(volatile std::atomic<T> *a, T expected, T desired)
{
  return a->compare_exchange_weak(expected, desired);
}

template<class T>
bool
cmpxch_update(std::atomic<T> *a, T *expected, T desired)
{
  return a->compare_exchange_weak(*expected, desired);
}

template<class T>
bool
cmpxch_update(volatile std::atomic<T> *a, T *expected, T desired)
{
  return a->compare_exchange_weak(*expected, desired);
}
