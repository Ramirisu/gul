//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <gul/config.hpp>

#include <gul/expected.hpp>

#include <gul/optional.hpp>

GUL_NAMESPACE_BEGIN

namespace detail {

template <typename Self>
GUL_CXX14_CONSTEXPR auto expected_value_to_optional_impl(std::true_type,
                                                         Self&& self)
    -> optional<typename remove_cvref_t<Self>::value_type>
{
  using SelfT = typename remove_cvref_t<Self>::value_type;
  if (self.has_value()) {
    return optional<SelfT>(in_place);
  } else {
    return optional<SelfT>();
  }
}

template <typename Self>
GUL_CXX14_CONSTEXPR auto expected_value_to_optional_impl(std::false_type,
                                                         Self&& self)
    -> optional<typename remove_cvref_t<Self>::value_type>
{
  using SelfT = typename remove_cvref_t<Self>::value_type;
  if (self.has_value()) {
    return optional<SelfT>(std::forward<Self>(self).value());
  } else {
    return optional<SelfT>();
  }
}

template <typename Self>
GUL_CXX14_CONSTEXPR auto expected_error_to_optional_impl(Self&& self)
    -> optional<typename remove_cvref_t<Self>::error_type>
{
  using SelfE = typename remove_cvref_t<Self>::error_type;
  if (self.has_value()) {
    return optional<SelfE>();
  } else {
    return optional<SelfE>(std::forward<Self>(self).error());
  }
}

}

template <typename T, typename E>
GUL_CXX14_CONSTEXPR auto expected<T, E>::value_to_optional() & -> optional<T>
{
  return detail::expected_value_to_optional_impl(std::is_void<T> {}, *this);
}

template <typename T, typename E>
GUL_CXX14_CONSTEXPR auto
expected<T, E>::value_to_optional() const& -> optional<T>
{
  return detail::expected_value_to_optional_impl(std::is_void<T> {}, *this);
}

template <typename T, typename E>
GUL_CXX14_CONSTEXPR auto expected<T, E>::value_to_optional() && -> optional<T>
{
  return detail::expected_value_to_optional_impl(std::is_void<T> {},
                                                 std::move(*this));
}

#if !defined(GUL_CXX_COMPILER_GCC48)
template <typename T, typename E>
GUL_CXX14_CONSTEXPR auto
expected<T, E>::value_to_optional() const&& -> optional<T>
{
  return detail::expected_value_to_optional_impl(std::is_void<T> {},
                                                 std::move(*this));
}
#endif

template <typename T, typename E>
GUL_CXX14_CONSTEXPR auto expected<T, E>::error_to_optional() & -> optional<E>
{
  return detail::expected_error_to_optional_impl(*this);
}

template <typename T, typename E>
GUL_CXX14_CONSTEXPR auto
expected<T, E>::error_to_optional() const& -> optional<E>
{
  return detail::expected_error_to_optional_impl(*this);
}

template <typename T, typename E>
GUL_CXX14_CONSTEXPR auto expected<T, E>::error_to_optional() && -> optional<E>
{
  return detail::expected_error_to_optional_impl(std::move(*this));
}

#if !defined(GUL_CXX_COMPILER_GCC48)
template <typename T, typename E>
GUL_CXX14_CONSTEXPR auto
expected<T, E>::error_to_optional() const&& -> optional<E>
{
  return detail::expected_error_to_optional_impl(std::move(*this));
}
#endif

GUL_NAMESPACE_END
