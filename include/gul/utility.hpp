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

template <class T, class U = T>
GUL_CXX14_CONSTEXPR T exchange(T& obj, U&& new_value) noexcept(
    std::is_nothrow_move_constructible<T>::value&&
        std::is_nothrow_assignable<T&, U>::value)
{
  T old_value = std::move(obj);
  obj = std::forward<U>(new_value);
  return std::move(old_value);
}

GUL_NAMESPACE_END
