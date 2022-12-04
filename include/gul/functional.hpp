//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <gul/config.hpp>

#include <gul/detail/functional.hpp>

#include <gul/type_traits.hpp>

GUL_NAMESPACE_BEGIN

template <typename F,
          typename... Args,
          GUL_REQUIRES(is_invocable<F, Args...>::value)>
constexpr auto invoke(F&& f, Args&&... args) noexcept(noexcept(
    detail::invoke_impl(std::forward<F>(f), std::forward<Args>(args)...)))
    -> decltype(detail::invoke_impl(std::forward<F>(f),
                                    std::forward<Args>(args)...))
{
  return detail::invoke_impl(std::forward<F>(f), std::forward<Args>(args)...);
}

template <typename R,
          typename F,
          typename... Args,
          GUL_REQUIRES(is_invocable_r<R, F, Args...>::value)>
constexpr auto invoke_r(F&& f, Args&&... args) noexcept(noexcept(
    detail::invoke_impl(std::forward<F>(f), std::forward<Args>(args)...))) -> R
{
  return detail::invoke_impl(std::forward<F>(f), std::forward<Args>(args)...);
}

GUL_NAMESPACE_END
