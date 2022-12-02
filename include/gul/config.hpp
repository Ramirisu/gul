//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <cassert>

#define GUL_NAMESPACE_BEGIN                                                    \
  namespace gul {                                                              \
  inline namespace v0 {

#define GUL_NAMESPACE_END                                                      \
  }                                                                            \
  }

#if defined(__cplusplus) && __cplusplus >= 201402L                             \
    || defined(_MSVC_LANG) && _MSVC_LANG >= 201402L
#define GUL_HAS_CXX14
#endif
#if defined(__cplusplus) && __cplusplus >= 201703L                             \
    || defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
#define GUL_HAS_CXX17
#endif
#if defined(__cplusplus) && __cplusplus >= 202002L                             \
    || defined(_MSVC_LANG) && _MSVC_LANG >= 202002L
#define GUL_HAS_CXX20
#endif

#define GUL_ASSERT(expr) assert(expr)
#define GUL_UNUSED(expr) ((void)expr)

// clang-format off
#if __cpp_exceptions >= 199711
#define GUL_NO_EXCEPTIONS 0
#define GUL_TRY try
#define GUL_CATCH(...) catch (__VA_ARGS__)
#define GUL_RETHROW() throw
#else
#define GUL_NO_EXCEPTIONS 1
#define GUL_TRY
#define GUL_CATCH(...) if (true) { } else
#define GUL_RETHROW() ((void)0)
#endif
// clang-format on

#if defined(__GNUC__) && !defined(__clang__)
#define GUL_CXX_COMPILER_GCC
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define GUL_CXX_COMPILER_MSVC
#endif

#if defined(GUL_CXX_COMPILER_GCC) && __GNUC__ < 5
#define GUL_CXX_COMPILER_GCC48
#endif

#if defined(GUL_CXX_COMPILER_MSVC) && 1900 <= _MSC_VER && _MSC_VER < 1910
#define GUL_CXX_COMPILER_MSVC2015
#endif

#if defined(GUL_HAS_CXX14) && !defined(GUL_CXX_COMPILER_MSVC2015)
#define GUL_CXX14_CONSTEXPR constexpr
#else
#define GUL_CXX14_CONSTEXPR
#endif
#ifdef GUL_HAS_CXX17
#define GUL_CXX17_INLINE inline
#else
#define GUL_CXX17_INLINE static
#endif
#ifdef GUL_HAS_CXX20
#define GUL_CXX20_EXPLICIT(expr) explicit(expr)
#define GUL_CXX20_CONSTEXPR constexpr
#else
#define GUL_CXX20_EXPLICIT(expr)
#define GUL_CXX20_CONSTEXPR
#endif
