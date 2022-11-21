//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <gul/config.hpp>

#include <utility>

GUL_NAMESPACE_BEGIN

struct in_place_t {
  explicit in_place_t() = default;
};

GUL_CXX17_INLINE constexpr in_place_t in_place {};

template <typename T, typename U = T>
GUL_CXX14_CONSTEXPR T exchange(T& obj, U&& new_value) noexcept(
    std::is_nothrow_move_constructible<T>::value&&
        std::is_nothrow_assignable<T&, U>::value)
{
  T old_value = std::move(obj);
  obj = std::forward<U>(new_value);
  return std::move(old_value);
}

template <typename T>
constexpr typename std::add_const<T>::type& as_const(T& t) noexcept
{
  return t;
}

template <typename T>
void as_const(const T&&) = delete;

template <typename Enum>
constexpr typename std::underlying_type<Enum>::type
to_underlying(Enum e) noexcept
{
  return static_cast<typename std::underlying_type<Enum>::type>(e);
}

namespace detail {
template <typename T, typename U>
struct cmp_impl {
  using UT = typename std::make_unsigned<T>::type;
  using UU = typename std::make_unsigned<U>::type;
  using ST = typename std::make_signed<T>::type;
  using SU = typename std::make_signed<U>::type;
  static GUL_CXX14_CONSTEXPR bool eq(UT t, UU u) noexcept
  {
    return t == u;
  }
  static GUL_CXX14_CONSTEXPR bool eq(ST t, SU u) noexcept
  {
    return t == u;
  }
  static GUL_CXX14_CONSTEXPR bool eq(ST t, UU u) noexcept
  {
    return t < 0 ? false : UT(t) == u;
  }
  static GUL_CXX14_CONSTEXPR bool eq(UT t, SU u) noexcept
  {
    return u < 0 ? false : t == UU(u);
  }
  static GUL_CXX14_CONSTEXPR bool lt(UT t, UU u) noexcept
  {
    return t < u;
  }
  static GUL_CXX14_CONSTEXPR bool lt(ST t, SU u) noexcept
  {
    return t < u;
  }
  static GUL_CXX14_CONSTEXPR bool lt(ST t, UU u) noexcept
  {
    return t < 0 ? true : UT(t) < u;
  }
  static GUL_CXX14_CONSTEXPR bool lt(UT t, SU u) noexcept
  {
    return u < 0 ? false : t < UU(u);
  }
};
}

template <typename T, typename U>
GUL_CXX14_CONSTEXPR bool cmp_equal(T t, U u) noexcept
{
  static_assert(std::is_integral<T>::value, "T must be integral type");
  static_assert(std::is_integral<U>::value, "U must be integral type");
  return detail::cmp_impl<T, U>::eq(t, u);
}

template <typename T, typename U>
GUL_CXX14_CONSTEXPR bool cmp_not_equal(T t, U u) noexcept
{
  static_assert(std::is_integral<T>::value, "T must be integral type");
  static_assert(std::is_integral<U>::value, "U must be integral type");
  return !cmp_equal(t, u);
}

template <typename T, typename U>
GUL_CXX14_CONSTEXPR bool cmp_less(T t, U u) noexcept
{
  static_assert(std::is_integral<T>::value, "T must be integral type");
  static_assert(std::is_integral<U>::value, "U must be integral type");
  return detail::cmp_impl<T, U>::lt(t, u);
}

template <typename T, typename U>
GUL_CXX14_CONSTEXPR bool cmp_greater(T t, U u) noexcept
{
  static_assert(std::is_integral<T>::value, "T must be integral type");
  static_assert(std::is_integral<U>::value, "U must be integral type");
  return detail::cmp_impl<T, U>::lt(u, t);
}

template <typename T, typename U>
GUL_CXX14_CONSTEXPR bool cmp_less_equal(T t, U u) noexcept
{
  static_assert(std::is_integral<T>::value, "T must be integral type");
  static_assert(std::is_integral<U>::value, "U must be integral type");
  return !cmp_greater(t, u);
}

template <typename T, typename U>
GUL_CXX14_CONSTEXPR bool cmp_greater_equal(T t, U u) noexcept
{
  static_assert(std::is_integral<T>::value, "T must be integral type");
  static_assert(std::is_integral<U>::value, "U must be integral type");
  return !cmp_less(t, u);
}

GUL_NAMESPACE_END
