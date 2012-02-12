#pragma once

template<class A, class B>
class pair {
 public:
  A _a;
  B _b;

  pair(const A &a, const B &b) : _a(a), _b(b) {}

  bool operator==(const pair<A, B> &other) {
    return _a == other._a && _b == other._b;
  }
};

template<class A, class B>
pair<A, B>
mkpair(const A &a, const B &b)
{
  return pair<A, B>(a, b);
}
