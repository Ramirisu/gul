//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <cassert>

#define DS_NAMESPACE_BEGIN                                                     \
  namespace ds {                                                               \
  inline namespace v0 {

#define DS_NAMESPACE_END                                                       \
  }                                                                            \
  }

#if defined(__cplusplus) && __cplusplus >= 201402L                             \
    || defined(_MSVC_LANG) && _MSVC_LANG >= 201402L
#define DS_HAS_CXX14
#endif
#if defined(__cplusplus) && __cplusplus >= 201703L                             \
    || defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
#define DS_HAS_CXX17
#endif
#if defined(__cplusplus) && __cplusplus >= 202002L                             \
    || defined(_MSVC_LANG) && _MSVC_LANG >= 202002L
#define DS_HAS_CXX20
#endif

#define DS_ASSERT(expr) assert(expr)
#define DS_UNUSED(expr) ((void)expr)

// clang-format off
#if __cpp_exceptions >= 199711
#define DS_NO_EXCEPTIONS 0
#define DS_TRY try
#define DS_CATCH(...) catch (__VA_ARGS__)
#define DS_RETHROW() throw
#else
#define DS_NO_EXCEPTIONS 1
#define DS_TRY
#define DS_CATCH(...) if (true) { } else
#define DS_RETHROW() ((void)0)
#endif
// clang-format on

#ifdef DS_HAS_CXX14
#define DS_CXX14_CONSTEXPR constexpr
#else
#define DS_CXX14_CONSTEXPR
#endif
#ifdef DS_HAS_CXX17
#define DS_CXX17_INLINE inline
#else
#define DS_CXX17_INLINE static
#endif
#ifdef DS_HAS_CXX20
#define DS_CXX20_EXPLICIT(expr) explicit(expr)
#define DS_CXX20_CONSTEXPR constexpr
#else
#define DS_CXX20_EXPLICIT(expr)
#define DS_CXX20_CONSTEXPR
#endif
