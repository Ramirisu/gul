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

GUL_NAMESPACE_END
