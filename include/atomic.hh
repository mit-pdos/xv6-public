#pragma once

#define _GLIBCXX_VISIBILITY(x)
#define _GLIBCXX_BEGIN_NAMESPACE_VERSION
#define _GLIBCXX_END_NAMESPACE_VERSION
#define _GLIBCXX_BEGIN_EXTERN_C extern "C" {
#define _GLIBCXX_END_EXTERN_C }
#define __glibcxx_assert(x)
#define __barrier() do { __asm__ __volatile__("" ::: "memory"); } while (0)

#define _GLIBCXX_ATOMIC_BUILTINS_1 1
#define _GLIBCXX_ATOMIC_BUILTINS_2 1
#define _GLIBCXX_ATOMIC_BUILTINS_4 1
#define _GLIBCXX_ATOMIC_BUILTINS_8 1

#include "atomic_std.h"
#include "atomic_util.hh"
