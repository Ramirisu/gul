//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <gul/config.hpp>

#include <cstddef>
#include <type_traits>

GUL_NAMESPACE_BEGIN

namespace detail {

using std::integral_constant;

struct in_place_t {
  explicit in_place_t() = default;
};

#if defined(GUL_HAS_CXX14) || defined(__cpp_lib_integer_sequence)

using std::integer_sequence;

using std::index_sequence;

using std::make_integer_sequence;

using std::make_index_sequence;

using std::index_sequence_for;

#else

template <typename T, T... Ints>
struct integer_sequence {
  using value_type = T;

  static constexpr std::size_t size() noexcept
  {
    return sizeof...(Ints);
  }
};

template <std::size_t... Ints>
using index_sequence = integer_sequence<std::size_t, Ints...>;

template <typename T, typename L, typename R>
struct make_integer_sequence_concat;

template <typename T, T... Ls, T... Rs>
struct make_integer_sequence_concat<T,
                                    integer_sequence<T, Ls...>,
                                    integer_sequence<T, Rs...>> {
  using type = integer_sequence<T, Ls..., (sizeof...(Ls) + Rs)...>;
};

template <typename T, typename Seq>
struct make_integer_sequence_impl;

template <typename T, T N>
struct make_integer_sequence_impl<T, integral_constant<T, N>> {
  using type = typename make_integer_sequence_concat<
      T,
      typename make_integer_sequence_impl<T, integral_constant<T, N / 2>>::type,
      typename make_integer_sequence_impl<T, integral_constant<T, N - N / 2>>::
          type>::type;
};

template <typename T>
struct make_integer_sequence_impl<T, integral_constant<T, T(0)>> {
  using type = integer_sequence<T>;
};

template <typename T>
struct make_integer_sequence_impl<T, integral_constant<T, T(1)>> {
  using type = integer_sequence<T, 0>;
};

template <typename T, T N>
using make_integer_sequence =
    typename make_integer_sequence_impl<T, integral_constant<T, N>>::type;

template <std::size_t N>
using make_index_sequence = make_integer_sequence<std::size_t, N>;

template <typename... T>
using index_sequence_for = make_index_sequence<sizeof...(T)>;

#endif
}

GUL_NAMESPACE_END
