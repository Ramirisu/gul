#pragma once

#include <cassert>

#define DS_ASSERT(expr) assert(expr)

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
