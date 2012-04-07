#pragma once

// A protected pointer to user data.  These act like pointers (and
// their in-memory representation is identical), but userptr's cannot
// be dereferenced without explicit checks.
template<typename T>
class userptr
{
  T* ptr;

public:
  explicit userptr(T* p) : ptr(p) { }
  explicit userptr(uptr p) : ptr((T*)p) { }
  userptr() = default;
  userptr(const userptr<T> &o) = default;
  userptr(userptr<T> &&o) = default;

  T* unsafe_get() const
  {
    return ptr;
  }

  // Allow implicit conversion to userptr<void> (mirroring C++'s
  // implicit casts to void*)
  operator userptr<void> () const
  {
    return userptr<void>{ptr};
  }

  // Allow implicit casts to uptr.  Often it makes sense to treat a
  // user pointer as an opaque number and a lot of existing code uses
  // this convention.  (XXX(austin) this means we can't have things
  // like operator+, which would probably subsume our current uses of
  // uptr anyway.)
  operator uptr () const
  {
    return (uptr)ptr;
  }
};

// For userptr to be passed like a regular pointer, its representation
// must be the same as a pointer (obviously) and, furthermore, the
// AMD64 ABI requires that it have a trivial copy construct and
// trivial destructor.
static_assert(sizeof(userptr<void>) == sizeof(void*), "userptr is wrong size");
static_assert(__is_pod(userptr<void>), "userptr is not a POD");
