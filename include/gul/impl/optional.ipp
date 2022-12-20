//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <gul/config.hpp>

#include <gul/optional.hpp>

#include <gul/expected.hpp>

GUL_NAMESPACE_BEGIN

namespace detail {

template <typename Self, typename E>
GUL_CXX14_CONSTEXPR auto
optional_to_expected_or_impl(std::true_type, Self&& self, E&& default_error)
    -> expected<typename remove_cvref_t<Self>::value_type, E>
{
  using SelfT = typename remove_cvref_t<Self>::value_type;
  if (self.has_value()) {
    return expected<SelfT, E>(in_place);
  } else {
    return expected<SelfT, E>(unexpect, std::forward<E>(default_error));
  }
}

template <typename Self, typename E>
GUL_CXX14_CONSTEXPR auto
optional_to_expected_or_impl(std::false_type, Self&& self, E&& default_error)
    -> expected<typename remove_cvref_t<Self>::value_type, E>
{
  using SelfT = typename remove_cvref_t<Self>::value_type;
  if (self.has_value()) {
    return expected<SelfT, E>(in_place, std::forward<Self>(self).value());
  } else {
    return expected<SelfT, E>(unexpect, std::forward<E>(default_error));
  }
}

}

template <typename T>
template <typename E>
GUL_CXX14_CONSTEXPR auto
optional<T>::to_expected_or(E&& default_error) & -> expected<T, E>
{
  return detail::optional_to_expected_or_impl(std::is_void<T> {}, *this,
                                              std::forward<E>(default_error));
}

template <typename T>
template <typename E>
GUL_CXX14_CONSTEXPR auto
optional<T>::to_expected_or(E&& default_error) const& -> expected<T, E>
{
  return detail::optional_to_expected_or_impl(std::is_void<T> {}, *this,
                                              std::forward<E>(default_error));
}

template <typename T>
template <typename E>
GUL_CXX14_CONSTEXPR auto
optional<T>::to_expected_or(E&& default_error) && -> expected<T, E>
{
  return detail::optional_to_expected_or_impl(
      std::is_void<T> {}, std::move(*this), std::forward<E>(default_error));
}

#if !defined(GUL_CXX_COMPILER_GCC48)
template <typename T>
template <typename E>
GUL_CXX14_CONSTEXPR auto
optional<T>::to_expected_or(E&& default_error) const&& -> expected<T, E>
{
  return detail::optional_to_expected_or_impl(
      std::is_void<T> {}, std::move(*this), std::forward<E>(default_error));
}
#endif

GUL_NAMESPACE_END
